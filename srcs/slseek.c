/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   slseek.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/06 14:14:10 by jjaniec           #+#    #+#             */
/*   Updated: 2020/01/10 20:11:04 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

int		slseek(t_ft_nm_file *file, int offset, int whence)
{
	int	r;

	r = -1;
	if (whence == SLSEEK_CUR)
	{
		if ((offset <= 0 && \
			offset < ((*file).seek_ptr - (*file).content)) || \
			(offset + ((*file).seek_ptr - (*file).content) <= (*file).totsiz))
		{
			(*file).seek_ptr += offset;
			r = offset;
		}
	}
	else if (whence == SLSEEK_SET)
	{
		if (offset >= 0 && (*file).totsiz >= offset)
		{
			(*file).seek_ptr = (*file).content + offset;
			r = offset;
		}
	}
	return (r);
}
