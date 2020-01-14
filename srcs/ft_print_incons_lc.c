/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_print_incons_lc.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/14 18:59:46 by jjaniec           #+#    #+#             */
/*   Updated: 2020/01/14 19:00:35 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

int		ft_print_incons_lc(void)
{
	ft_putstr_fd(OTOOL_ERR_PREFIX "Inconsistent load command size" \
		" / number of sections in LC_SEGMENT\n", ERR_FD);
	return (0);
}
