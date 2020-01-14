/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_otool.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/20 18:07:35 by jjaniec           #+#    #+#             */
/*   Updated: 2020/01/14 18:52:35 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

int		main(int ac, char **av)
{
	char			*file_path;
	char			*file_content;
	t_ft_nm_file	file;
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
		file_path = av[i];
		if ((file.totsiz = read_file_content(file_path, &file_content)) == -1)
			return (1);
		file.filepath = file_path;
		file.content = file_content;
		file.seek_ptr = file.content;
		ft_otool_process_file(&file);
	}
}
