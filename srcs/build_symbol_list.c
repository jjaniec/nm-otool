/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_symbol_list.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/06 16:29:17 by jjaniec           #+#    #+#             */
/*   Updated: 2020/01/03 18:17:05 by jjaniec          ###   ########.fr       */
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

/*
** Append sym to linked list and return new elem ptr,
** also update of the list ptr if new elem should be placed
** at the start
*/

static t_ft_nm_sym	*append_sym_to_list(t_ft_nm_sym **list, uint64_t symvalue, char **symname, char symtype, uint8_t n_type)
{
	t_ft_nm_sym		*new;
	t_ft_nm_sym		*e;
	t_ft_nm_sym		*prev;

	e = *list;
	if (!(new = malloc(sizeof(t_ft_nm_sym))))
		return (NULL);
	new->symvalue = symvalue;
	new->symname = (symname[0]) ? (symname[0]) : ("");
	new->symtype = symtype;
	new->indr_name = symname[1];
	new->n_type = n_type;
	prev = NULL;
	while (e && ft_strcmp(new->symname, e->symname) >= 0 && \
		!(((new->symvalue < e->symvalue) || e->indr_name) && \
		ft_strcmp(new->symname, e->symname) == 0))
	{
		prev = e;
		e = e->next;
	}
	if (prev)
	{
		prev->next = new;
		new->next = e;
	}
	else
	{
		new->next = *list;
		*list = new;
	}
	return (new);
}

static char		*safe_read_symname(t_ft_nm_file *file, char *strtab_offset, \
				unsigned int index, uint32_t strsize, bool handle_corrupt_syms)
{
	size_t		original_offset;

	original_offset = file->seek_ptr - file->content;
	if (index > strsize && !handle_corrupt_syms)
	{
		ft_putstr_fd(file->filepath, 2);
		ft_putstr_fd(": truncated or malformed object (bad string table index past the end of string table for symbol)", 2);
		return (NULL);
	}
	if (slseek(file, (int)(&strtab_offset[index] - file->content), SLSEEK_SET) == -1)
		return (NULL);
	return ((&strtab_offset[index]));
}

static char		*get_indr_name(t_ft_nm_file *file, uint64_t n_value, char *strtab_offset, struct symtab_command *symtabcmd)
{
	if (n_value == 0)
		return ("");
	if (n_value > symtabcmd->strsize || \
		slseek(file, (int)(&strtab_offset[n_value] - file->content), \
		SLSEEK_SET) == -1)
		return ("bad string index");
	return (strtab_offset + n_value);
}

/*
** Build symbol list and return beginning of the list
*/

static t_ft_nm_sym		*build_symbol_list_64(t_ft_nm_file *file, t_ft_nm_hdrinfo *hdrinfo, struct symtab_command *symtabcmd, bool handle_corrupt_syms)
{
	uint32_t			i;
	struct nlist_64		*symtab;
	struct nlist_64		nl;
	char				*strtab;
	char				*symname;
	char				*indr_name;
	t_ft_nm_sym			*list = NULL;
	char				type;

	i = 0;
	dprintf(2, "Build symbol list 64, size of struct %zu\n", sizeof(struct nlist_64));
	strtab = hdrinfo->fat_offset + symtabcmd->stroff + (char *)file->content;
	slseek(file, hdrinfo->fat_offset + symtabcmd->symoff, SLSEEK_SET);
	symtab = (struct nlist_64 *)file->seek_ptr;
	while (i < symtabcmd->nsyms)
	{
		if (-1 == slseek(file, (int)((char *)&symtab[i] - file->content), SLSEEK_SET))
		{
			dprintf(2, "slseek() failed\n");
			exit(1);
		}
		sseek_read(file, &nl, sizeof(struct nlist_64));
		i++;
		if (nl.n_type & N_STAB)
			type = '-';
		else
			type = get_symbol_type(nl.n_type, nl.n_sect, nl.n_value, hdrinfo);
		indr_name = (!(nl.n_type & N_STAB) && (nl.n_type & N_TYPE) == N_INDR) ? (get_indr_name(file, nl.n_value, strtab, symtabcmd)) : (NULL);
		if ((nl.n_type & N_EXT) && type != '?')
			type = toupper(type);
		if (!(symname = safe_read_symname(file, strtab, nl.n_un.n_strx, symtabcmd->strsize, handle_corrupt_syms)) && !handle_corrupt_syms)
			return (free_symbol_list(list));
		append_sym_to_list(&list, nl.n_value, (char *[2]){symname, indr_name}, type, nl.n_type);
	}
	return (list);
}

static t_ft_nm_sym		*build_symbol_list_32(t_ft_nm_file *file, t_ft_nm_hdrinfo *hdrinfo, struct symtab_command *symtabcmd, bool handle_corrupt_syms)
{
	uint32_t			i;
	struct nlist		*symtab;
	struct nlist		nl;
	char				*strtab;
	char				*symname;
	char				*indr_name;
	t_ft_nm_sym			*list = NULL;
	char				type;

	i = 0;
	dprintf(2, "Build symbol list 32\n");
	strtab = hdrinfo->fat_offset + symtabcmd->stroff + (char *)file->content;
	slseek(file, hdrinfo->fat_offset + symtabcmd->symoff, SLSEEK_SET);
	symtab = (struct nlist *)file->seek_ptr;
	while (i < symtabcmd->nsyms)
	{
		if (-1 == slseek(file, (int)((char *)&symtab[i] - file->content), SLSEEK_SET))
		{
			dprintf(2, "slseek() failed\n");
			exit(1);
		}
		sseek_read(file, &nl, sizeof(struct nlist));
		i++;
		if (nl.n_type & N_STAB)
			type = '-';
		else
			type = get_symbol_type(nl.n_type, nl.n_sect, nl.n_value, hdrinfo);
		indr_name = (!(nl.n_type & N_STAB) && (nl.n_type & N_TYPE) == N_INDR) ? (get_indr_name(file, nl.n_value, strtab, symtabcmd)) : (NULL);
		if ((nl.n_type & N_EXT) && type != '?')
		    type = toupper(type);
		if (!(symname = safe_read_symname(file, strtab, nl.n_un.n_strx, symtabcmd->strsize, handle_corrupt_syms)) && !handle_corrupt_syms)
			return (free_symbol_list(list));
		append_sym_to_list(&list, nl.n_value, (char *[2]){symname, indr_name}, type, nl.n_type);
	}
	return (list);
}

t_ft_nm_sym				*build_symbol_list(t_ft_nm_hdrinfo *hdrinfo, bool handle_corrupt_syms)
{
	slseek(hdrinfo->file, hdrinfo->fat_offset + hdrinfo->machhdr_size, SLSEEK_SET);
	if (goto_load_command(hdrinfo->file, hdrinfo, (uint32_t [3]){LC_SYMTAB, 0}, NULL) != -1)
	{
		if (hdrinfo->is_64)
			return (build_symbol_list_64(\
				hdrinfo->file, hdrinfo, \
				(void *)hdrinfo->file->seek_ptr - sizeof(struct load_command), handle_corrupt_syms));
		return (build_symbol_list_32(\
			hdrinfo->file, hdrinfo, \
			(void *)hdrinfo->file->seek_ptr - sizeof(struct load_command), handle_corrupt_syms));
	}
	return (NULL);
}
