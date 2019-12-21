/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dump_symlist.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joffreyjaniec <joffreyjaniec@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/20 16:29:19 by jjaniec           #+#    #+#             */
/*   Updated: 2019/12/21 15:21:44 by joffreyjani      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

/*
** Loop through syms and output with the correct format
*/

static bool		should_print_address(t_ft_nm_sym *s)
{
	if ((s->symvalue && \
			!((s->n_type & N_TYPE) == N_INDR)) || \
		(s->n_type & N_TYPE) == N_ABS || \
		(s->n_type & N_TYPE) == N_SECT)
		return (true);
	return (false);
}

static void		print_address(t_ft_nm_hdrinfo *hdrinfo, t_ft_nm_sym *s)
{
	if (hdrinfo->is_64)
	{
		ft_printf((should_print_address(s)) ? \
			("%016" PRIx64 "") : ("%16c"), \
			(should_print_address(s)) ? (s->symvalue) : (' '));
	}
	else
	{
		ft_printf((should_print_address(s)) ? \
			("%08" PRIx64 "") : ("%8c"), \
			(should_print_address(s)) ? (s->symvalue) : (' '));
	}
}

void		dump_symlist(t_ft_nm_hdrinfo *hdrinfo, t_ft_nm_sym *symlist)
{
	t_ft_nm_sym		*s;

	s = symlist;
	while (s)
	{
		if (s->symtype != '-')
		{
			print_address(hdrinfo, s);
			ft_printf(" %c %s", s->symtype, s->symname);
			if (s->indr_name)
				ft_printf(" (indirect for %s)", s->indr_name);
			ft_printf("\n");
		}
		s = s->next;
	}
}
