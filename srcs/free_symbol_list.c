/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_symbol_list.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/03 18:14:14 by jjaniec           #+#    #+#             */
/*   Updated: 2020/01/03 18:16:12 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

void		*free_symbol_list(t_ft_nm_sym *syms)
{
	if (syms)
	{
		free_symbol_list(syms->next);
		free(syms);
	}
	return (NULL);
}
