/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_fat_arch_values.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/14 19:02:03 by jjaniec           #+#    #+#             */
/*   Updated: 2020/01/18 15:02:10 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

int				init_fat_arch_values(t_ft_nm_hdrinfo *hdrinfo, \
					struct fat_arch *arch, uint32_t fat_magic)
{
	if (arch)
	{
		hdrinfo->fat_offset = is_big_endian(fat_magic) ? (arch->offset) : \
			swap_32bit(arch->offset);
		hdrinfo->fat_align = 1 << (is_big_endian(fat_magic) ? (arch->align) : \
			swap_32bit(arch->align));
		hdrinfo->fat_size = is_big_endian(fat_magic) ? (arch->size) : \
			swap_32bit(arch->size);
		if (hdrinfo->fat_offset % hdrinfo->fat_align)
		{
			ft_putstr_fd("Header not aligned on it's alignment\n", ERR_FD);
			return (1);
		}
	}
	else
	{
		hdrinfo->fat_offset = 0;
		hdrinfo->fat_align = 0;
		hdrinfo->fat_size = hdrinfo->file->totsiz;
	}
	return (0);
}
