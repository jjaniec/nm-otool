/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sseek_read.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/06 14:13:49 by jjaniec           #+#    #+#             */
/*   Updated: 2019/12/06 14:13:57 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

int			sseek_read(t_ft_nm_file *file, void *buf, unsigned int size)
{
	unsigned int	i;

	i = 0;
	while (file->seek_ptr < (file->content + file->totsiz) && i < size)
	{
		*((unsigned char *)buf + i) = *((unsigned char *)file->seek_ptr);
		file->seek_ptr += 1;
		i++;
	}
	return (i);
}
