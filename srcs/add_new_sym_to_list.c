/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   add_new_sym_to_list.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/13 21:27:59 by jjaniec           #+#    #+#             */
/*   Updated: 2020/01/13 21:28:11 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

/*
** Append sym to linked list and return new elem ptr,
** also update of the list ptr if new elem should be placed
** at the start
*/

static t_ft_nm_sym	*sort_append_sym_to_list(t_ft_nm_sym **list, \
						t_ft_nm_sym *new)
{
	t_ft_nm_sym		*e;
	t_ft_nm_sym		*prev;

	e = *list;
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

t_ft_nm_sym			*add_new_sym_to_list(t_ft_nm_sym **list, \
						uint64_t symvalue, char **symname, \
						char *types)
{
	t_ft_nm_sym		*new;

	if (!(new = malloc(sizeof(t_ft_nm_sym))))
		return (NULL);
	new->symvalue = symvalue;
	new->symname = (symname[0]) ? (symname[0]) : ("");
	new->indr_name = symname[1];
	new->symtype = types[0];
	new->n_type = types[1];
	return (sort_append_sym_to_list(list, new));
}
