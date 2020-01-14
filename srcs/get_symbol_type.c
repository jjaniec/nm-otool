/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_symbol_type.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/14 19:06:23 by jjaniec           #+#    #+#             */
/*   Updated: 2020/01/14 19:06:29 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

char			get_symbol_type(uint8_t type, uint8_t sect, \
						uint64_t value, \
						t_ft_nm_hdrinfo *hdrinfo)
{
	type = type & N_TYPE;
	if (type == N_UNDF)
	{
		if (value)
			return ('c');
		return ('u');
	}
	if (type == N_ABS)
		return ('a');
	if (type == N_SECT)
	{
		if (sect == hdrinfo->text_nsect)
			return ('t');
		else if (sect == hdrinfo->data_nsect)
			return ('d');
		else if (sect == hdrinfo->bss_nsect)
			return ('b');
		return ('s');
	}
	if (type == N_PBUD)
		return ('u');
	if (type == N_INDR)
		return ('i');
	return ('?');
}
