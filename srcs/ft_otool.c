/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_otool.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/20 18:07:35 by jjaniec           #+#    #+#             */
/*   Updated: 2020/01/18 15:20:32 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

static int		read_and_process_file(char *filename)
{
	char			*file_content;
	t_ft_nm_file	file;

	if ((file.totsiz = read_file_content(filename, &file_content)) == -1)
	{
		free(file_content);
		return (1);
	}
	file.filepath = filename;
	file.content = file_content;
	file.seek_ptr = file.content;
	ft_otool_process_file(&file);
	free(file_content);
	return (0);
}

int				main(int ac, char **av)
{
	unsigned int	i;

	if (!(ac > 1))
	{
		ft_putstr_fd("ft_otool: at least one file must be specified\n", \
			STDERR_FILENO);
		return (1);
	}
	i = 0;
	while (++i != (unsigned int)ac)
	{
		if (read_and_process_file(av[1]) == 1)
			return (1);
	}
	return (0);
}
