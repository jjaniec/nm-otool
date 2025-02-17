/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_header_info.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/06 14:27:45 by jjaniec           #+#    #+#             */
/*   Updated: 2020/01/18 13:34:14 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

static t_ft_nm_hdrinfo	*init_macho_header_cmds(t_ft_nm_hdrinfo *hdrinfo)
{
	if (sseek_read(hdrinfo->file, &(hdrinfo->cpu_type), \
			sizeof(cpu_type_t)) != sizeof(cpu_type_t) || \
		sseek_read(hdrinfo->file, &(hdrinfo->cpu_subtype), \
			sizeof(cpu_subtype_t)) != sizeof(cpu_subtype_t))
		return (NULL);
	if (!hdrinfo->is_be)
		hdrinfo->cpu_type = swap_32bit(hdrinfo->cpu_type);
	if (!hdrinfo->is_be)
		hdrinfo->cpu_subtype = swap_32bit(hdrinfo->cpu_subtype);
	hdrinfo->text_nsect = 0;
	hdrinfo->data_nsect = 0;
	hdrinfo->bss_nsect = 0;
	hdrinfo->next = NULL;
	if (read_byte_range_at_pos(hdrinfo, &hdrinfo->ncmds, sizeof(uint32_t), \
			hdrinfo->fat_offset + ((hdrinfo->is_64) ? \
			(__offsetof(struct mach_header_64, ncmds)) : \
			(__offsetof(struct mach_header, ncmds)))) || \
		read_byte_range_at_pos(hdrinfo, &hdrinfo->sizeofcmds, sizeof(uint32_t),\
			hdrinfo->fat_offset + ((hdrinfo->is_64) ? \
			(__offsetof(struct mach_header_64, sizeofcmds)) : \
			(__offsetof(struct mach_header, sizeofcmds)))))
		return (NULL);
	hdrinfo->machhdr_size = (hdrinfo->is_64) ? \
		(sizeof(struct mach_header_64)) : (sizeof(struct mach_header));
	return (hdrinfo);
}

static t_ft_nm_hdrinfo	*init_macho_header(t_ft_nm_file *file, \
							t_ft_nm_hdrinfo *hdrinfo, \
							struct fat_arch *arch, uint32_t start_magic)
{
	uint32_t			magic;

	hdrinfo->file = file;
	if (init_fat_arch_values(hdrinfo, arch, start_magic))
		return (NULL);
	if (arch)
	{
		slseek(file, hdrinfo->fat_offset, SLSEEK_SET);
		sseek_read(file, &magic, sizeof(uint32_t));
	}
	else
		magic = start_magic;
	hdrinfo->magic = magic;
	hdrinfo->is_64 = is_magic_64(magic);
	hdrinfo->is_be = is_big_endian(magic);
	slseek(hdrinfo->file, hdrinfo->fat_offset + sizeof(uint32_t), SLSEEK_SET);
	return (init_macho_header_cmds(hdrinfo));
}

static int				fill_next_fat_header_infos(t_ft_nm_file *file, \
							uint32_t *fat_header_idx, uint32_t magic, \
							t_ft_nm_hdrinfo **headers)
{
	struct fat_arch		arch;
	t_ft_nm_hdrinfo		*cur_mach_header;
	t_ft_nm_hdrinfo		*new_mach_header;

	cur_mach_header = headers[0];
	new_mach_header = headers[1];
	slseek(file, sizeof(struct fat_header) + (sizeof(struct fat_arch) * \
		*fat_header_idx), SLSEEK_SET);
	sseek_read(file, &arch, sizeof(struct fat_arch));
	if (!init_macho_header(file, new_mach_header, &arch, magic))
		return (1);
	if (*fat_header_idx && \
		check_hdr_overlap(cur_mach_header, (uint32_t)file->seek_ptr))
	{
		ft_putstr_fd("Overlap of previous header\n", ERR_FD);
		return (2);
	}
	if (new_mach_header->fat_offset % new_mach_header->fat_align)
	{
		ft_putstr_fd("Header not aligned on it's alignment\n", ERR_FD);
		return (1);
	}
	*fat_header_idx += 1;
	return (0);
}

static int				handle_fat_header(t_ft_nm_file *file, \
							t_ft_nm_hdrinfo *hdrinfo, uint32_t nfat_arch, \
							uint32_t magic)
{
	uint32_t			fat_header_idx;
	t_ft_nm_hdrinfo		*cur_mach_header;
	t_ft_nm_hdrinfo		*new_mach_header;

	fat_header_idx = 0;
	while (fat_header_idx < nfat_arch)
	{
		if (fat_header_idx)
		{
			new_mach_header = malloc(sizeof(t_ft_nm_hdrinfo));
			if (fill_next_fat_header_infos(file, &fat_header_idx, magic, \
				(t_ft_nm_hdrinfo *[2]){cur_mach_header, new_mach_header}))
				return (1);
			cur_mach_header->next = new_mach_header;
			cur_mach_header = new_mach_header;
		}
		else
		{
			if (fill_next_fat_header_infos(file, &fat_header_idx, magic, \
				(t_ft_nm_hdrinfo *[2]){NULL, hdrinfo}))
				return (1);
			cur_mach_header = hdrinfo;
		}
	}
	return (0);
}

int						init_header_info(t_ft_nm_file *file, \
							t_ft_nm_hdrinfo *hdrinfo)
{
	uint32_t			magic;
	uint32_t			nfat_arch;

	hdrinfo->next = NULL;
	sseek_read(file, &magic, sizeof(uint32_t));
	if (is_magic_fat(magic))
	{
		sseek_read(file, &nfat_arch, sizeof(uint32_t));
		if (!is_big_endian(magic))
			nfat_arch = swap_32bit(nfat_arch);
		return (handle_fat_header(file, hdrinfo, nfat_arch, magic));
	}
	else if (is_magic_mach(magic))
		return (init_macho_header(file, hdrinfo, NULL, magic) == NULL);
	ft_putstr_fd("The file was not recognized as a valid object file\n", 2);
	return (1);
}
