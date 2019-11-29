/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_nm.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/29 16:09:55 by jjaniec           #+#    #+#             */
/*   Updated: 2019/11/29 16:19:51 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

static char		*read_file_content(char *file)
{
	int		fd;

	if ((fd = open(file, O_RDONLY)) != -1)
	{
		return (ft_read_content(fd));
	}
	return (NULL);
}

int				ft_nm(char *file)
{
	char	*file_content;

	if (!(file_content = read_file_content(file)))
		return (1);
	printf("File content %s\n", file_content);
	return (0);
}
