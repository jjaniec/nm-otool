/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_symbol_list.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/06 16:29:17 by jjaniec           #+#    #+#             */
/*   Updated: 2019/12/07 19:13:01 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

static char		get_symbol_type(uint8_t type, uint8_t sect, uint64_t value, t_ft_nm_hdrinfo *hdrinfo)
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
	// printf("Type not found: %u\n", type);
	// fflush(stdout);
	return ('?');
}

/*
** Append sym to linked list and return new elem ptr,
** also update of the list ptr if new elem should be placed
** at the start
*/

static t_ft_nm_sym	*append_sym(t_ft_nm_sym **list, uint64_t symvalue, char *symname, char symtype)
{
	t_ft_nm_sym		*new;
	t_ft_nm_sym		*e;
	t_ft_nm_sym		*prev;

	e = *list;
	// if (!(e && e->symname))
	// 	return (NULL);
	if (!(new = malloc(sizeof(t_ft_nm_sym))))
		return (NULL);
	new->symvalue = symvalue;
	new->symname = symname;
	new->symtype = symtype;
	prev = NULL;
	while (e && ft_strcmp(new->symname, e->symname) >= 0)
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

char			*safe_read_symname(t_ft_nm_file *file, const char *strtab_offset, unsigned int index)
{
	char		*symname;
	char		symname_buf[256];
	int			r;
	off_t		original_offset;

	symname = NULL;
	original_offset = file->seek_ptr - file->content;
	// printf("aaaa %lu %lu - size: %zu - index: %u\n", &strtab_offset[index] - file->content, file->seek_ptr - file->content, file->totsiz, index);
	// return &strtab_offset[index];
	if (slseek(file, (off_t)(&strtab_offset[index] - file->content) /*- file->content*/, SLSEEK_SET) == -1)
	{
		// printf("bbb %u\n", strtab_offset);
		return (symname);
	}
	// slseek(file, original_offset, SLSEEK_SET);
	// return &strtab_offset[index];

	if ((r = sseek_read(file, &symname_buf, 256)) != -1)
	{
		if ((file->seek_ptr - file->content) != (long)file->totsiz)
		{
			symname = ft_strdup(symname_buf);
			// slseek(file, -r, SLSEEK_CUR);
		}
	}
	slseek(file, original_offset, SLSEEK_SET);
	return (symname);
}

/*
** Build symbol list and return beginning of the list
*/

t_ft_nm_sym		*build_symbol_list(t_ft_nm_file *file, t_ft_nm_hdrinfo *hdrinfo, struct symtab_command *symtabcmd)
{
	uint32_t			i;
	struct nlist_64		*symtab;
	struct nlist_64		nl;
	const char*			strtab;
	char				*symname;
	t_ft_nm_sym			*list = NULL;
	off_t				strtab_offset;
	char				type;

	i = 0;
	// strtab_offset = hdrinfo->offset + symtabcmd->stroff;
	strtab = hdrinfo->offset + symtabcmd->stroff + (char *)file->content;
	slseek(file, hdrinfo->offset + symtabcmd->symoff, SLSEEK_SET);
	symtab = (struct nlist_64 *)file->seek_ptr;
	while (i < symtabcmd->nsyms)
	{
		slseek(file, &symtab[i], SLSEEK_SET);
		sseek_read(file, &nl, sizeof(struct nlist_64));
		i++;
		if (nl.n_type & N_STAB)
			type = '-';
		else
			type = get_symbol_type(nl.n_type, nl.n_sect, nl.n_value, hdrinfo);
		if ((nl.n_type & N_EXT) && type != '?')
		    type = toupper(type);
		if (!(symname = safe_read_symname(file, strtab, nl.n_un.n_strx)))
			exit(1);
		//symname = &strtab[nl.n_un.n_strx];
		append_sym(&list, nl.n_value, symname, type);
	}
	return (list);
}

t_ft_nm_sym		*build_symbol_list_32(t_ft_nm_file *file, t_ft_nm_hdrinfo *hdrinfo, struct symtab_command *symtabcmd)
{
	uint32_t			i;
	struct nlist		*symtab;
	struct nlist		nl;
	const char*			strtab;
	t_ft_nm_sym			*list = NULL;
	char				type;

	i = 0;
	strtab = hdrinfo->offset + symtabcmd->stroff + (char *)file->content;
	slseek(file, hdrinfo->offset + symtabcmd->symoff, SLSEEK_SET);
	symtab = (struct nlist *)file->seek_ptr;
	while (i < symtabcmd->nsyms)
	{
		slseek(file, &symtab[i], SLSEEK_SET);
		sseek_read(file, &nl, sizeof(struct nlist));
		i++;
		if (nl.n_type & N_STAB)
			type = '-';
		else
			type = get_symbol_type(nl.n_type, nl.n_sect, nl.n_value, hdrinfo);
		if ((nl.n_type & N_EXT) && type != '?')
		    type = toupper(type);
		const char* symname = &strtab[nl.n_un.n_strx];
		append_sym(&list, nl.n_value, symname, type);
	}
	return (list);
}
