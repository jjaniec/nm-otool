/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   is_big_endian.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/29 18:02:16 by jjaniec           #+#    #+#             */
/*   Updated: 2019/12/06 20:56:11 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

bool		is_big_endian(uint32_t magic)
{
	return (magic == MH_MAGIC || magic == MH_MAGIC_64 || \
			magic == FAT_MAGIC || magic == FAT_MAGIC_64);
}
