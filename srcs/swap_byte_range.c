/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   swap_byte_range.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/29 19:05:46 by jjaniec           #+#    #+#             */
/*   Updated: 2019/11/29 19:57:04 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

void		swap_byte_range(void *bytes, size_t range)
{
	size_t		i;
	char		tmp;

	i = 0;
	while (i < range)
	{
		tmp = *((unsigned char *)bytes + i);
		*((unsigned char *)bytes + i) = *((unsigned char *)bytes + i + 1);
		*((unsigned char *)bytes + i + 1) = tmp;
		i += 2;
	}
}
