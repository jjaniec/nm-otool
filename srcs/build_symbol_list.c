/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_symbol_list.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/06 16:29:17 by jjaniec           #+#    #+#             */
/*   Updated: 2020/01/13 21:29:17 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

static char		get_symbol_type(uint8_t type, uint8_t sect, uint64_t value, \
					t_ft_nm_hdrinfo *hdrinfo)
{
	type = type & N_TYPE;
	if (type == N_UNDF)
	{
		if (value)
			return ('c');
		return ('u');
	}
	if (type == N_ABS)
		return ('a');
	if (type == N_SECT)
	{
		if (sect == hdrinfo->text_nsect)
			return ('t');
		else if (sect == hdrinfo->data_nsect)
			return ('d');
		else if (sect == hdrinfo->bss_nsect)
			return ('b');
		return ('s');
	}
	if (type == N_PBUD)
		return ('u');
	if (type == N_INDR)
		return ('i');
	return ('?');
}

static char		*safe_read_symname(t_ft_nm_hdrinfo *hdrinfo, \
					unsigned int index, bool handle_corrupt_syms, \
					struct symtab_command *symtabcmd)
{
	size_t		original_offset;
	uint32_t	strsize;
	char		*strtab;

	original_offset = hdrinfo->file->seek_ptr - hdrinfo->file->content;
	strtab = hdrinfo->fat_offset + symtabcmd->stroff + \
		(char *)hdrinfo->file->content;
	strsize = symtabcmd->strsize;
	if (index > strsize && !handle_corrupt_syms)
	{
		ft_putstr_fd(hdrinfo->file->filepath, 2);
		ft_putstr_fd(": truncated or malformed object " \
			"(bad string table index past the end " \
			"of string table for symbol)", 2);
		return (NULL);
	}
	if (slseek(hdrinfo->file, (int)(&strtab[index] - \
		hdrinfo->file->content), SLSEEK_SET) == -1)
		return (NULL);
	return ((&strtab[index]));
}

static char		*get_indr_name(t_ft_nm_file *file, uint64_t n_value, \
					char *strtab_offset, struct symtab_command *symtabcmd)
{
	if (n_value == 0)
		return ("");
	if (n_value > symtabcmd->strsize || \
		slseek(file, (int)(&strtab_offset[n_value] - file->content), \
		SLSEEK_SET) == -1)
		return ("bad string index");
	return (strtab_offset + n_value);
}

static t_ft_nm_sym	*append_next_sym_64(t_ft_nm_hdrinfo *hdrinfo, \
						struct symtab_command *symtabcmd, \
						t_ft_nm_sym **list, bool handle_corrupt_syms)
{
	char				*strtab;
	char				type;
	char				*symname;
	char				*indr_name;
	struct nlist_64		nl;

	strtab = hdrinfo->fat_offset + symtabcmd->stroff + \
		(char *)hdrinfo->file->content;
	sseek_read(hdrinfo->file, &nl, sizeof(struct nlist_64));
	if (nl.n_type & N_STAB)
		type = '-';
	else
		type = get_symbol_type(nl.n_type, nl.n_sect, nl.n_value, hdrinfo);
	indr_name = (!(nl.n_type & N_STAB) && (nl.n_type & N_TYPE) == N_INDR) ? \
		(get_indr_name(hdrinfo->file, nl.n_value, strtab, symtabcmd)) : (NULL);
	if ((nl.n_type & N_EXT) && type != '?')
		type = toupper(type);
	if (!(symname = safe_read_symname(hdrinfo, nl.n_un.n_strx, \
		handle_corrupt_syms, symtabcmd)) && !handle_corrupt_syms)
		return (free_symbol_list(*list));
	add_new_sym_to_list(list, nl.n_value, (char *[2]){symname, \
		indr_name}, (char[2]){type, nl.n_type});
	return (*list);
}

static t_ft_nm_sym	*append_next_sym_32(t_ft_nm_hdrinfo *hdrinfo, \
						struct symtab_command *symtabcmd, \
						t_ft_nm_sym **list, bool handle_corrupt_syms)
{
	char				*strtab;
	char				type;
	char				*symname;
	char				*indr_name;
	struct nlist		nl;

	strtab = hdrinfo->fat_offset + symtabcmd->stroff + \
		(char *)hdrinfo->file->content;
	sseek_read(hdrinfo->file, &nl, sizeof(struct nlist));
	if (nl.n_type & N_STAB)
		type = '-';
	else
		type = get_symbol_type(nl.n_type, nl.n_sect, nl.n_value, hdrinfo);
	indr_name = (!(nl.n_type & N_STAB) && (nl.n_type & N_TYPE) == N_INDR) ? \
		(get_indr_name(hdrinfo->file, nl.n_value, strtab, symtabcmd)) : (NULL);
	if ((nl.n_type & N_EXT) && type != '?')
		type = toupper(type);
	if (!(symname = safe_read_symname(hdrinfo, nl.n_un.n_strx, \
		handle_corrupt_syms, symtabcmd)) && !handle_corrupt_syms)
		return (free_symbol_list(*list));
	add_new_sym_to_list(list, nl.n_value, (char *[2]){symname, \
		indr_name}, (char[2]){type, nl.n_type});
	return (*list);
}

/*
** Build symbol list and return beginning of the list
*/

static t_ft_nm_sym		*build_symbol_list_64(t_ft_nm_file *file, \
							t_ft_nm_hdrinfo *hdrinfo, \
							struct symtab_command *symtabcmd, \
							bool handle_corrupt_syms)
{
	uint32_t			i;
	struct nlist_64		*symtab;
	t_ft_nm_sym			*list;

	i = 0;
	list = NULL;
	slseek(file, hdrinfo->fat_offset + symtabcmd->symoff, SLSEEK_SET);
	symtab = (struct nlist_64 *)file->seek_ptr;
	while (i < symtabcmd->nsyms)
	{
		if (-1 == slseek(file, (int)((char *)&symtab[i] - \
			file->content), SLSEEK_SET))
		{
			dprintf(2, "slseek() failed\n");
			exit(1);
		}
		i++;
		if (!append_next_sym_64(hdrinfo, symtabcmd, &list, handle_corrupt_syms))
			return (NULL);
	}
	return (list);
}

static t_ft_nm_sym		*build_symbol_list_32(t_ft_nm_file *file, \
							t_ft_nm_hdrinfo *hdrinfo, \
							struct symtab_command *symtabcmd, \
							bool handle_corrupt_syms)
{
	uint32_t			i;
	struct nlist		*symtab;
	t_ft_nm_sym			*list;

	i = 0;
	list = NULL;
	slseek(file, hdrinfo->fat_offset + symtabcmd->symoff, SLSEEK_SET);
	symtab = (struct nlist *)file->seek_ptr;
	while (i < symtabcmd->nsyms)
	{
		if (-1 == slseek(file, (int)((char *)&symtab[i] - \
			file->content), SLSEEK_SET))
		{
			dprintf(2, "slseek() failed\n");
			exit(1);
		}
		i++;
		if (!append_next_sym_32(hdrinfo, symtabcmd, &list, handle_corrupt_syms))
			return (NULL);
	}
	return (list);
}

t_ft_nm_sym				*build_symbol_list(t_ft_nm_hdrinfo *hdrinfo, \
							bool handle_corrupt_syms)
{
	slseek(hdrinfo->file, hdrinfo->fat_offset + hdrinfo->machhdr_size, \
		SLSEEK_SET);
	if (goto_load_command(hdrinfo->file, hdrinfo, \
		(uint32_t[3]){LC_SYMTAB, 0}, NULL) != -1)
	{
		if (hdrinfo->is_64)
			return (build_symbol_list_64(\
				hdrinfo->file, hdrinfo, \
				(void *)hdrinfo->file->seek_ptr - sizeof(struct load_command), \
				handle_corrupt_syms));
			return (build_symbol_list_32(\
				hdrinfo->file, hdrinfo, \
				(void *)hdrinfo->file->seek_ptr - sizeof(struct load_command), \
				handle_corrupt_syms));
	}
	return (NULL);
}
