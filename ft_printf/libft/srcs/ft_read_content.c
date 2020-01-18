/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_read_content.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/11/28 15:16:18 by jjaniec           #+#    #+#             */
/*   Updated: 2020/01/18 14:31:33 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

/*
** Append to ft_read_content's buffer, following of file descriptor, with a
** size of ($BUFF_SIZE * $bufsize_multiplier) bytes
*/

static int	ft_resize_buf(int fd, char **buf, int *bufsize_multiplier,
							int *total_len)
{
	char	*buf2;
	int		buf2_len;
	char	*new_buf;

	buf2 = malloc(256 * (*bufsize_multiplier));
	buf2_len = read(fd, buf2, 256 * (*bufsize_multiplier));
	new_buf = malloc(sizeof(char) * (*total_len + buf2_len));
	ft_memcpy(new_buf, *buf, *total_len);
	ft_memcpy(new_buf + *total_len, buf2, buf2_len);
	*total_len += buf2_len;
	free(buf2);
	free(*buf);
	*buf = new_buf;
	return (buf2_len == 256 * (*bufsize_multiplier));
}

/*
** Read the first $BUFF_SIZE bytes of file descriptor passed in parameter
** and store it in a malloc'ed char *
*/

char		*ft_read_content(int fd, int *read_total)
{
	char	*buf;
	int		x;
	int		multiplier;

	multiplier = 1;
	buf = malloc(256);
	if ((x = read(fd, buf, 256)) == -1)
	{
		free(buf);
		return (NULL);
	}
	*read_total = x;
	if (x < 256)
		return (buf);
	while (1)
	{
		multiplier++;
		if (!ft_resize_buf(fd, &buf, &multiplier, &x))
			break ;
	}
	*read_total = x;
	return (buf);
}
