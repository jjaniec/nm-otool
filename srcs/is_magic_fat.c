/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   is_magic_fat.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/06 22:27:07 by jjaniec           #+#    #+#             */
/*   Updated: 2019/12/06 22:27:58 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

bool		is_magic_fat(uint32_t magic)
{
	return (magic == FAT_CIGAM_64 || magic == FAT_MAGIC_64 || \
			magic == FAT_MAGIC || magic == FAT_CIGAM);
}
