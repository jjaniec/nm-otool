/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_header_info.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/06 14:27:45 by jjaniec           #+#    #+#             */
/*   Updated: 2020/01/11 17:53:09 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

static int			init_fat_arch_values(t_ft_nm_hdrinfo *hdrinfo, \
						struct fat_arch *arch, uint32_t fat_magic)
{
	if (arch)
	{
		hdrinfo->fat_offset = is_big_endian(fat_magic) ? (arch->offset) : \
			swap_32bit(arch->offset);
		hdrinfo->fat_align = 1 << (is_big_endian(fat_magic) ? (arch->align) : \
			swap_32bit(arch->align));
		hdrinfo->fat_size = is_big_endian(fat_magic) ? (arch->size) : \
			swap_32bit(arch->size);
	}
	else
	{
		hdrinfo->fat_offset = 0;
		hdrinfo->fat_align = 0;
		hdrinfo->fat_size = hdrinfo->file->totsiz;
	}
	return (0);
}

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

	init_fat_arch_values(hdrinfo, arch, start_magic);
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
	// if (!init_macho_header_cmds(hdrinfo))
	// 	return (NULL);
	// dprintf(2, "Init new header: magic: %x, is_be: %d, is_64: %d, cpu_type: %u, subtype: %x / %u, fat_offset %u, size: %zu, fat_size: %u, fat_align: %u, ncdms: %u - sizeofcmds: %u\n", \
	// 	hdrinfo->magic, hdrinfo->is_be, hdrinfo->is_64, hdrinfo->cpu_type, (hdrinfo->cpu_subtype & 0xf), CPU_SUBTYPE_X86_64_ALL, hdrinfo->fat_offset, hdrinfo->machhdr_size, hdrinfo->fat_size, hdrinfo->fat_align, hdrinfo->ncmds, hdrinfo->sizeofcmds);
	// return (hdrinfo);
}

static int			handle_fat_header(t_ft_nm_file *file, \
						t_ft_nm_hdrinfo *hdrinfo, uint32_t nfat_arch, \
						uint32_t magic)
{
	struct fat_arch		arch;
	uint32_t			fat_header_idx;
	t_ft_nm_hdrinfo		*cur_mach_header;
	t_ft_nm_hdrinfo		*new_mach_header;

	fat_header_idx = 0;
	cur_mach_header = NULL;
	while (fat_header_idx < nfat_arch)
	{
		slseek(file, sizeof(struct fat_header) + (sizeof(struct fat_arch) * fat_header_idx), SLSEEK_SET);
		sseek_read(file, &arch, sizeof(struct fat_arch));
		if (cur_mach_header)
		{
			new_mach_header = malloc(sizeof(t_ft_nm_hdrinfo));
			new_mach_header->file = file;
			if (!init_macho_header(file, new_mach_header, &arch, magic))
				return (1);
			if (check_hdr_overlap(cur_mach_header, (uint32_t)file->seek_ptr))
			{
				dprintf(2, "Overlap of previous header\n");
				return (2);
			}
			cur_mach_header->next = new_mach_header;
			cur_mach_header = new_mach_header;
		}
		else
		{
			if (!(cur_mach_header = init_macho_header(file, hdrinfo, &arch, magic)))
				return (1);
		}
		if (cur_mach_header->fat_offset % cur_mach_header->fat_align)
		{
			dprintf(ERR_FD, "Header not aligned on it's alignment");
			return (2);
		}
		fat_header_idx++;
		dprintf(2, "fat_header_idx: %d / %" PRIu32 " - cur_mach_header: %p, offset: %x\n", fat_header_idx, nfat_arch, cur_mach_header, cur_mach_header->fat_offset);
	}
	return (0);
}

int					init_header_info(t_ft_nm_file *file, \
						t_ft_nm_hdrinfo *hdrinfo)
{
	uint32_t			magic;
	uint32_t			nfat_arch;

	hdrinfo->file = file;
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
