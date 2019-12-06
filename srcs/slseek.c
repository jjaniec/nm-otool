/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   slseek.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/06 14:14:10 by jjaniec           #+#    #+#             */
/*   Updated: 2019/12/06 14:14:30 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

off_t		slseek(t_ft_nm_file *file, off_t offset, int whence)
{
	off_t	r;

	r = -1;
	if (whence == SLSEEK_CUR)
	{
		if ((offset <= 0 && \
			(size_t)offset < (size_t)((*file).seek_ptr - (*file).content)) || \
			((size_t)offset + (size_t)((*file).seek_ptr - (*file).content) <= (*file).totsiz))
		{
			(*file).seek_ptr += offset;
			r = offset;
		}
	}
	else if (whence == SLSEEK_SET)
	{
		if (offset >= 0 && (*file).totsiz >= (size_t)offset)
		{
			(*file).seek_ptr = (*file).content + offset;
			r = offset;
		}
	}
	return (r);
}
