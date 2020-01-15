/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_symbol_list.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/06 16:29:17 by jjaniec           #+#    #+#             */
/*   Updated: 2020/01/15 18:37:08 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

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

static t_ft_nm_sym	*build_symbol_list_64(t_ft_nm_file *file, \
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
			ft_putstr_fd("Invalid nsyms count, exiting\n", 2);
			return (NULL);
		}
		i++;
		if (!append_next_sym_64(hdrinfo, symtabcmd, &list, handle_corrupt_syms))
			return (NULL);
	}
	return (list);
}

static t_ft_nm_sym	*build_symbol_list_32(t_ft_nm_file *file, \
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
			ft_putstr_fd("Invalid nsyms count, exiting\n", 2);
			return (NULL);
		}
		i++;
		if (!append_next_sym_32(hdrinfo, symtabcmd, &list, handle_corrupt_syms))
			return (NULL);
	}
	return (list);
}

t_ft_nm_sym			*build_symbol_list(t_ft_nm_hdrinfo *hdrinfo, \
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
