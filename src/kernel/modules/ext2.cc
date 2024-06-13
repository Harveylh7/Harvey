#include <core/os.h>
#include <ext2.h>

File* ext2_mount(char* name,u32 flag,File* dev){
    int ret=ext2_check_disk(dev);
    if(ret!=RETURN_OK){
        io.print("ext2: can't mount %s in %s \n",dev->getName(),name);
        return NULL;
    }
    else{
        io.print("ext2:  mount %s in %s \n",dev->getName(),name);
        Ext2* ret=new Ext2(name);
        ret->ext2inode = EXT2_INUM_ROOT;
        ext2_get_disk_info(dev,ret);
        ret->scan();
        return ret;
    }
}


module("module.ext2",MODULE_FILESYSTEM,Ext2,ext2_mount)



Ext2::Ext2(char* n) : File(n,TYPE_DIRECTORY)
{
	map=NULL;
}


void Ext2::scan(){
    ext2_scan(this);
}

u32 Ext2::close(){
    return NOT_DEFINED;
}


u32	Ext2::open(u32 flag){
	ext2_inode *inodee=ext2_read_inode(disk,ext2inode);
	map=ext2_read_file(disk,inodee);
	kfree(inodee);
	return RETURN_OK;
}


u32 Ext2::read(u32 pos,u8* buffer,u32 sizee){
    u32 bufsize=sizee;
    if((pos + bufsize) > (size))
    {
        bufsize = (u32)(size) - pos;
    }
    memcpy((char*)buffer,(char *)(map + pos),bufsize);
    return bufsize;
}



u32	Ext2::write(u32 pos,u8* buffer,u32 sizee){
	return NOT_DEFINED;
}

u32	Ext2::ioctl(u32 id,u8* buffer){
	return NOT_DEFINED;
}

u32	Ext2::remove(){
	delete this;
	return RETURN_OK;
}





// 在文件系统中，超级块的位置是固定的，超级块总是位于文件系统的第1024字节处
// 读取超级块的数据
int ext2_read_sb(File *dev, ext2_super_block *sb)
{
    if (dev != NULL)
    {
        dev->read((u32)1024, (u8 *)sb, sizeof(ext2_super_block));
        return RETURN_OK;
    }
    else
    {
        return ERROR_PARAM;
    }
}

// 读取group块的数据
int ext2_read_gd(File *fdev, ext2_group_desc *gd, ext2_disk *info)
{
    if (fdev != NULL)
    {
        u32 offset;
        // 超级块占据了第1024到第2047字节的位置,所以组描述符表从第2048字节处位置开始
        offset = ((info->blocksize == 1024) ? 2048 : info->blocksize);
        int gd_size = (info->groups * ((int)sizeof(struct ext2_group_desc)));
        fdev->read(offset, (u8 *)gd, gd_size);
        return RETURN_OK;
    }
    else
    {
        return ERROR_PARAM;
    }
}

void ext2_get_disk_info(File *dev, Ext2 *fp)
{
    ext2_disk *info = (ext2_disk *)kmalloc(sizeof(ext2_disk));
    info->sb = (ext2_super_block *)kmalloc(sizeof(ext2_super_block));
    info->dev = dev;
    int i, j;

    ext2_read_sb(dev, info->sb);
    // 1024是基础块的大小
    info->blocksize = 1024 << ((info->sb)->s_log_block_size); // s_log_block_size存放的是2的对数

    // 如果块组数少于 inode 组数，则某些 inode 将无法被管理，反之亦然。
    // 通过比较块组数和 inode 组数，取其中较大的值作为文件系统的总块组数，确保所有的块和 inode 都有对应的块组进行管理。

    i = (info->sb->s_blocks_count / info->sb->s_blocks_per_group) + ((info->sb->s_blocks_count % info->sb->s_blocks_per_group) ? 1 : 0); // 余数不足则补一块
    j = (info->sb->s_inodes_count / info->sb->s_inodes_per_group) + ((info->sb->s_inodes_count % info->sb->s_inodes_per_group) ? 1 : 0);

    info->groups = (i > j) ? i : j; // 取较大的
    int gd_size = info->group * ((int)sizeof(ext2_group_desc));
    info->gd = (ext2_group_desc *)kmalloc(gd_size);
    ext2_read_gd(info->dev, info->gd, info);
    fp->disk = info;
    return;
}

int ext2_check_disk(File *dev)
{
    
    ext2_super_block *sb=(ext2_super_block *)kmalloc(sizeof(ext2_super_block));
	if (ext2_read_sb(dev,sb)!=RETURN_OK)
		return ERROR_PARAM;
	if (sb->s_magic==0xEF53){//Ext文件系统的标识是0xEF53
		kfree(sb);
		return RETURN_OK;
	}
	else{
		kfree(sb);
		return ERROR_PARAM;
	}
}


ext2_inode *ext2_read_inode(ext2_disk* hd, int i_num)
{
	int gr_num, index;
	u32 offset;
	ext2_inode *inode;
	ext2_group_desc * info=hd->gd;
	//超级块里面记录了总的ext2_inode数
	inode = (ext2_inode *) kmalloc((hd->sb)->s_inode_size);//sizeof(ext2_inode));////超级块里面记录了总的ext2_inode数
	gr_num = (i_num - 1) / ((hd->sb)->s_inodes_per_group);
	index = (i_num - 1) % ((hd->sb)->s_inodes_per_group);
	offset = (info[gr_num].bg_inode_table * (hd->blocksize)) + (index * ((hd->sb)->s_inode_size));//第 gr_num 个组的组描述符。
	if ((hd->dev)!=NULL){
		(hd->dev)->read(offset,(u8*) inode,(hd->sb)->s_inode_size);
	}
	return inode;
}

int ext2_is_directory(Ext2 *fp)
{
    ext2_inode *inod = ext2_read_inode(fd->disk,fp->ext2inode);
    int ret=(inod->i_mode & EXT2_S_IFDIR)? 1 :0;
    kfree(inod);
    return ret;
}

char *ext2_read_file(ext2_disk *hd,ext2_inode *inode)
{
    File *dev = hd->dev;

    char *mmap_base, *mmap_head, *buf;

    int *p, *pp, *ppp;
    int i,j,k;
    int n,size;

    buf = (char *)kmalloc(hd->blocksize);
    p = (int *)kmalloc(hd->blocksize);
    pp = (int *)kmalloc(hd->blocksize);
    ppp = (int *) kmalloc(hd->blocksize);


    size = inode->i_size;//inode中存储文件的大小
    mmap_head = mmap_base = (char *)kmalloc(size);

    for(i = 0; i < 12 && inode->i_block[i];++i)
    {
        dev->read((u32)(inode->i_block[i] * hd->blocksize),(u8*) buf, (hd->blocksize));
        n = ((size > (int)hd->blocksize)?(int)hd->blocksize : size);
        memcpy(mmap_head,buf,n);
        mmap_head += n;
        size -= n;
    }

    if(inode->i_block[12])
    {
        

    }

}







