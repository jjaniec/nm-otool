/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_hdr_overlap.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/13 15:24:52 by jjaniec           #+#    #+#             */
/*   Updated: 2019/12/13 18:39:04 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

static int		align(int x, int align)
{
	while (x % align)
		x++;
	return (x);
}

int				check_hdr_overlap(t_ft_nm_hdrinfo *hdrinfo, int offset)
{
	if (offset > align(hdrinfo->fat_align + hdrinfo->fat_size, hdrinfo->fat_align))
		return (0);
	return (1);
}
