/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_byte_range_at_pos.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/14 17:47:58 by jjaniec           #+#    #+#             */
/*   Updated: 2019/12/14 17:57:30 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

int			read_byte_range_at_pos(t_ft_nm_hdrinfo *hdrinfo, void *buffer, \
				unsigned int size, int offset)
{
	if (!(slseek(hdrinfo->file, offset, SLSEEK_SET) >= 0 && \
		sseek_read(hdrinfo->file, buffer, size) == (int)size))
		return (1);
	if (!hdrinfo->is_be)
		swap_byte_range(buffer, size);
	return (0);
}
