/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_file_content.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/20 18:08:37 by jjaniec           #+#    #+#             */
/*   Updated: 2020/01/18 14:30:32 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

int		read_file_content(char *file, char **content)
{
	int			fd;
	int			len;

	len = -1;
	if ((fd = open(file, O_RDONLY)) != -1)
	{
		if (!(*content = ft_read_content(fd, &len)))
			return (-1);
		close(fd);
	}
	return (len);
}
