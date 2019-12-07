/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   is_magic_mach.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/07 17:51:17 by jjaniec           #+#    #+#             */
/*   Updated: 2019/12/07 17:53:26 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

bool		is_magic_mach(uint32_t magic)
{
	return (magic == MH_MAGIC || magic == MH_MAGIC_64 || \
			magic == MH_CIGAM || magic == MH_CIGAM_64);
}
