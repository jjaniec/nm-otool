/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/11/29 15:45:19 by jjaniec           #+#    #+#             */
/*   Updated: 2019/11/29 19:55:39 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

static char			*read_file_content(char *file)
{
	int		fd;
	char	*content;

	if ((fd = open(file, O_RDONLY)) != -1)
	{
		content = ft_read_content(fd);
		close(fd);
		return (content);
	}
	return (NULL);
}

static char			*check_args(int ac, char **av)
{
	if (ac > 1)
		return (av[1]);
}

int					main(int ac, char **av)
{
	char	*file_path;
	char	*file_content;
	t_ft_nm_file	file;

	if (!(file_path = check_args(ac, av)))
	{
		file_path = "a.out";
	}
	if (!(file_content = read_file_content(file_path)))
		return (1);
	file.totsiz = ft_strlen(file_content);
	file.seek_ptr = file_content;
	file.content = file_content;
	return (ft_nm(&file));
}
