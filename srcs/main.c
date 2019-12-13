/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/29 15:45:19 by jjaniec           #+#    #+#             */
/*   Updated: 2019/12/13 18:31:07 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

static int		read_file_content(char *file, char **content)
{
	int			fd;
	int			len;

	len = -1;
	if ((fd = open(file, O_RDONLY)) != -1)
	{
		*content = ft_read_content(fd, &len);
		close(fd);
	}
	return (len);
}

static char			*check_args(int ac, char **av)
{
	if (ac > 1)
		return (av[1]);
	return "a.out";
}

int					main(int ac, char **av)
{
	char	*file_path;
	char	*file_content;
	t_ft_nm_file	file;

	file_path = check_args(ac, av);
	if ((file.totsiz = read_file_content(file_path, &file_content)) == -1)
		return (1);
	dprintf(2, "Read file total size: %d\n", file.totsiz);
	file.content = file_content;
	file.seek_ptr = file.content;
	return (ft_nm(&file));
}
