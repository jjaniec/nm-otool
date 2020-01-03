/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_load_commands.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/13 20:29:00 by jjaniec           #+#    #+#             */
/*   Updated: 2020/01/02 20:16:59 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

static int		check_segment_command_64(t_ft_nm_hdrinfo *hdrinfo, \
					struct segment_command_64 *seg, uint32_t *parsed_filesize)
{
	if ((seg->fileoff - *parsed_filesize) % 4096 && \
		!(seg->fileoff == (hdrinfo->machhdr_size + hdrinfo->sizeofcmds)))
	{
		ft_putstr_fd("Inconsistent file offsets / sizes in segment_command_64\n", 2);
		return (1);
	}
	if (seg->cmdsize < (sizeof(struct segment_command_64) + seg->nsects * sizeof(struct section_64)))
	{
		ft_putstr_fd("Inconsistent cmdsize in LC_SEGMENT_64 for the number of sections\n", 2);
		return (1);
	}
	*parsed_filesize = *parsed_filesize + seg->filesize;
	return (0);
}

static int		check_segment_command_32(t_ft_nm_hdrinfo *hdrinfo, \
					struct segment_command *seg, uint32_t *parsed_filesize)
{
	if (seg->fileoff != *parsed_filesize)
	{
		ft_putstr_fd("Inconsistent file offsets / sizes in segment_command_32\n", 2);
		return (1);
	}
	if (seg->cmdsize < (sizeof(struct segment_command) + seg->nsects * sizeof(struct section)))
	{
		ft_putstr_fd("Inconsistent cmdsize in LC_SEGMENT for the number of sections\n", 2);
		return (1);
	}
	*parsed_filesize = *parsed_filesize + seg->filesize;
	return (0);
}

static int		check_segment_command(t_ft_nm_hdrinfo *hdrinfo, \
					struct segment_command *seg, uint32_t *parsed_filesize)
{
	if (hdrinfo->is_64)
		return (check_segment_command_64(hdrinfo, seg, parsed_filesize));
	return (check_segment_command_32(hdrinfo, seg, parsed_filesize));
}

static int		check_symtab_command(t_ft_nm_hdrinfo *hdrinfo, \
					struct symtab_command *symtab)
{
	uint32_t		symsize;

	symsize = (symtab->nsyms * ( \
		(hdrinfo->is_64) ? (sizeof(struct nlist_64)) : (sizeof(struct nlist)) \
	));
	if (!(symsize <= (symtab->stroff - symtab->symoff)))
	{
		ft_putstr_fd(hdrinfo->file->filepath, 2);
		ft_putstr_fd(" truncated or malformed object (string table overlaps symbol table)\n", 2);
		return (1);
	}
	return (0);
}


static int		check_load_commands_64(t_ft_nm_file *file, \
					t_ft_nm_hdrinfo *hdrinfo)
{
	uint32_t			cmdsize;
	uint32_t			parsed_ncmds;
	uint32_t			parsed_sizeofcmds;
	uint32_t			parsed_filesize = 0;
	uint32_t			cmd;

	(void)file;
	parsed_ncmds = 0;
	parsed_sizeofcmds = 0;
	parsed_filesize = 0;
	while (parsed_ncmds < hdrinfo->ncmds && \
			parsed_sizeofcmds < hdrinfo->sizeofcmds)
	{
		read_byte_range_at_pos(hdrinfo, &cmd, sizeof(uint32_t), \
			hdrinfo->fat_offset + hdrinfo->machhdr_size + \
			parsed_sizeofcmds + \
			__offsetof(struct load_command, cmd));
		read_byte_range_at_pos(hdrinfo, &cmdsize, sizeof(uint32_t), \
			hdrinfo->fat_offset + hdrinfo->machhdr_size + \
			parsed_sizeofcmds + \
			__offsetof(struct load_command, cmdsize));
		slseek(file, hdrinfo->fat_offset + hdrinfo->machhdr_size + \
			parsed_sizeofcmds, SLSEEK_SET);
		parsed_sizeofcmds += cmdsize;
		parsed_ncmds++;
		if ((cmd == LC_SEGMENT || cmd == LC_SEGMENT_64))
		{
			if (check_segment_command(hdrinfo, (void *)file->seek_ptr, &parsed_filesize))
				return (2);
		}
		else if (cmd == LC_SYMTAB)
		{
			if (check_symtab_command(hdrinfo, (void *)file->seek_ptr))
				return (2);
		}
	}
	dprintf(DEBUG_FD, "Parsed ncmds: %u / %u - parsed_size: %u / %u - parsed_filesize: %u / %u / %u\n", parsed_ncmds, hdrinfo->ncmds, parsed_sizeofcmds, hdrinfo->sizeofcmds, parsed_filesize, hdrinfo->file->totsiz, hdrinfo->fat_size);
	if (!(parsed_filesize <= hdrinfo->fat_size))
	{
		ft_putstr_fd("Inconsistent file offsets / sizes in segment commands\n", 2);
		return (2);
	}
	return (!(parsed_ncmds == hdrinfo->ncmds && \
			parsed_sizeofcmds <= hdrinfo->sizeofcmds));
}

int			check_load_commands(t_ft_nm_file *file, t_ft_nm_hdrinfo *hdrinfo)
{
	int		r;

	slseek(file, hdrinfo->fat_offset + hdrinfo->machhdr_size, SLSEEK_SET);
	if ((r = check_load_commands_64(file, hdrinfo)) == 1)
		dprintf(ERR_FD, "Invalid load commands size / number\n");
	return (r);
}
