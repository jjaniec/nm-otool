/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   build_symbol_list.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/06 16:29:17 by jjaniec           #+#    #+#             */
/*   Updated: 2019/12/06 17:35:01 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

static char		*get_symbol_type(uint32_t type)
{
	if (type == N_UNDF)
		return "U";
	if (type == N_ABS)
		return "TODO";
	if (type == N_SECT)
		return "T";
	if (type == N_PBUD)
		return "TODO";
	if (type == N_INDR)
		return "TODO";
	return NULL;
}

/*
** Append sym to linked list and return new elem ptr,
** also update of the list ptr if new elem should be placed
** at the start
*/

t_ft_nm_sym		*append_sym(t_ft_nm_sym **list, uint64_t symvalue, char *symname, char symtype)
{
	t_ft_nm_sym		*new;
	t_ft_nm_sym		*e;
	t_ft_nm_sym		*prev;

	if (!(new = malloc(sizeof(t_ft_nm_sym))))
		return (NULL);
	new->symvalue = symvalue;
	new->symname = symname;
	new->symtype = symtype;
	e = *list;
	prev = NULL;
	while (e && ft_strcmp(new->symname, e->symname) > 0)
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

/*
** Build symbol list and return beginning of the list
*/

t_ft_nm_sym		*build_symbol_list(t_ft_nm_file *file, t_ft_nm_hdrinfo *fileinfo, struct symtab_command *symtabcmd)
{
	uint32_t			i;
	struct nlist_64		*symtab;
	struct nlist_64		nl;
	const char* strtab;
	t_ft_nm_sym			*list = NULL;

	i = 0;
	strtab = symtabcmd->stroff + (char *)file->content;
	slseek(file, symtabcmd->symoff, SLSEEK_SET);
	symtab = (struct nlist_64 *)file->seek_ptr;
	// printf("%u symtabcmd offset - nsysms: %u\n", symtabcmd->symoff, symtabcmd->nsyms);
	while (i < symtabcmd->nsyms)
	{
		slseek(file, &symtab[i], SLSEEK_SET);
		sseek_read(file, &nl, sizeof(struct nlist_64));
		i++;
		if (nl.n_type & N_STAB)
			continue;
		const char* type = get_symbol_type(nl.n_type & N_TYPE);
		const char* symname = &strtab[nl.n_un.n_strx];
		append_sym(&list, nl.n_value, symname, type[0]);
	}
	return (list);
}
