/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   goto_load_command.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joffreyjaniec <joffreyjaniec@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/06 16:11:24 by jjaniec           #+#    #+#             */
/*   Updated: 2019/12/21 19:43:46 by joffreyjani      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>


/*
** Loop through int tab,
** return found index or -1 if x is not in tab
*/

static int		indexof_uint(uint32_t x, uint32_t *tab, unsigned int tab_size)
{
	unsigned int	i;

	i = 0;
	while (x != tab[i] && i < tab_size)
		i++;
	return (i < tab_size ? (i) : -1);
}

/*
** After placing the seek pointer of the file at the mach-o header,
** use this to safely read through the file & fing a load_command by a list
** of load command codes specified in load_cmds.
** Return -1 if load commands in the load_cmds list were not found,
** else return index of the load command type in the load_cmds list
** & fill cmd struct
*/

int		goto_load_command(t_ft_nm_file *file, t_ft_nm_hdrinfo *hdrinfo, uint32_t load_cmds[2], struct load_command *cmd)
{
	off_t			seek_ptr_save;
	uint32_t		i;
	int				idx;

	i = 0;
	while (i < hdrinfo->ncmds)
	{
		seek_ptr_save = file->seek_ptr - file->content;
		sseek_read(file, cmd, sizeof(struct load_command));
		if (!hdrinfo->is_be)
			swap_byte_range(cmd, sizeof(struct load_command));
		dprintf(2, "Load command %u seek: %lx - size: %u - .cmd %u ?= %u ?= %u\n", \
			i, file->seek_ptr - file->content, cmd->cmdsize, cmd->cmd, load_cmds[0], load_cmds[1]);
		if ((idx = indexof_uint(cmd->cmd, load_cmds, 2)) != -1)
			return (idx);
		slseek(file, seek_ptr_save + \
			cmd->cmdsize, \
			SLSEEK_SET);
		i++;
	}
	return (-1);
}
