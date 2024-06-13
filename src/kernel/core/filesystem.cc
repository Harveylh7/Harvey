#include <core/os.h>

Filesystem::Filesystem() {}

void Filesystem::init()
{
    // 创建一个根目录
    root = new File("/", TYPE_DIRECTORY);

    dev = root->createChild("dev", TYPE_DIRECTORY);

    root->createChild("proc", TYPE_DIRECTORY);
    root->createChild("mnt", TYPE_DIRECTORY); // 存储挂载设备

    File *sysd = root->createChild("sys", TYPE_DIRECTORY);
    var = sysd->createChild("env", TYPE_DIRECTORY);
    sysd->createChild("usr", TYPE_DIRECTORY);
    sysd->createChild("mods", TYPE_DIRECTORY);
    sysd->createChild("sockets", TYPE_DIRECTORY);
}

Filesystem::~Filesystem()
{
    delete root; // delete会调用File的析构函数
}

// 创建一个设备节点（或称为特殊文件）
void Filesystem::mknod(char *module, char *name, u32 flag)
{
    modm.createDevice(module, name, flag);
}

File *Filesystem::getRoot()
{
    return root;
}

File *Filesystem::path(char *p)
{ // 根据路径获取文件File*
    if (!p)
        return NULL;
    // 建立一个文件指针
    File *fp = root;
    char *name, *beg_p, *end_p;

    // fp开始的文件
    if (p[0] == '/')
    {
        fp = root;
    }
    else
    {
        if (arch.pcurrent != NULL)
        {
            fp = (arch.pcurrent)->getCurrentDir();
        }
    }

    beg_p = p;
    while (*beg_p == '/')
        ++beg_p;
    end_p = beg_p + 1;

    while (*beg_p != 0)
    {
        if (fp->getType() != TYPE_DIRECTORY) // 如果前面一个不是目录，则表示出错了
            return NULL;

        while (*end_p != 0 && *end_p != '/')
            ++end_p; // 找到0的位置或者找到‘/’的位置

        // 获取当前beg_p和end_p中间的文件
        name = (char *)kmalloc(end_p - beg_p + 1);
        memcpy(name, beg_p, end_p - beg_p);
        name[end_p - beg_p] = 0;

        // 将当前的名字进行比较
        if (strcmp(name, "..") == 0)
        {
            fp = fp->getParent();
        }
        else if (strcmp(name, ".") == 0)
        {
        }
        else
        {
            // 在fp目录下找，是否能找到路径中end_p 和beg_p之间的文件，保证路径的正确性
            fp->scan();                 // 确保文件是最新的
            if (!(fp = fp->find(name))) // 更新fp的指向
            {
                kfree(name);
                return 0;
            }

            if (fp->getType() == TYPE_LINK)
            {
                fp = fp->getLink();
            }
        }
        beg_p = end_p;
        while (*beg_p == '/')
            ++beg_p;
        end_p = beg_p + 1;
        kfree(name);
    }
    return fp;
}

File *Filesystem::pivot_root(File *targetdir)
{
    File *newRoot = new File("/", TYPE_DIRECTORY);
    File *mainChild = targetdir->getChild();
    newRoot->addChild(mainChild);
    s8 i = 1;
    // 获取子节点的前一个兄弟节点
    File *tempChild = mainChild->getPrec();
    do
    {
        if (tempChild == NULL)
        {
            i = 0;
        }
        else
        {
            newRoot->addChild(tempChild);
            tempChild = tempChild->getPrec();
        }
    } while (i == 1);
    tempChild = mainChild->getNext();
    i = 1;
    do
    {
        if (tempChild == NULL)
            i = 0;
        else
        {
            newRoot->addChild(tempChild);
            tempChild = tempChild->getNext();
        }
    } while (i == 1);
    return newRoot;
}

// 找到name文件，没找到就返回父节点以及没找到的name文件名
File *Filesystem::path_parent(char *p, char *fname)
{
    // if (!p)
    // 	return NULL;
    // File *ofp;
    // File *fp = root;
    // char *name, *beg_p, *end_p;

    // if (p[0] == '/'){
    // 	fp = root;
    // }
    // else
    // {
    //     if (arch.pcurrent != NULL)
    //     {
    //         fp = (arch.pcurrent)->getCurrentDir();
    //     }
    // }

    // beg_p = p;
    // while (*beg_p == '/')
    // 	beg_p++;
    // end_p = beg_p + 1;

    // while (*beg_p != 0)
    // {
    // 	if (fp->getType() != TYPE_DIRECTORY)
    // 		return NULL;

    // 	while (*end_p != 0 && *end_p != '/')
    // 		end_p++;
    // 	name = (char *)kmalloc(end_p - beg_p + 1);
    // 	memcpy(name, beg_p, end_p - beg_p);
    // 	name[end_p - beg_p] = 0;

    // 	if (strcmp("..", name) == 0)
    // 	{ // '..'
    // 		fp = fp->getParent();
    // 	}
    // 	else if (strcmp(".", name) == 0)
    // 	{ // '.'
    // 	}
    // 	else
    // 	{
    // 		ofp = fp;

    // 		if (fp->getType() == TYPE_LINK && (fp->getLink() != NULL))
    // 		{
    // 			fp = fp->getLink();
    // 		}

    // 		if (!(fp = fp->find(name)))
    // 		{
    // 			strcpy(fname, name);
    // 			kfree(name);
    // 			return ofp;
    // 		}
    // 	}

    // 	beg_p = end_p;
    // 	while (*beg_p == '/')
    // 		beg_p++;
    // 	end_p = beg_p + 1;

    // 	kfree(name);
    // }
    // return fp;

    if (!p)
        return NULL;

    File *ofp;
    File *fp = root;
    char *name, *beg_p, *end_p;

    if (p[0] == '/')
        fp = root;
    else
    {
        if (arch.pcurrent != NULL) /* prend de le dossier actuel du fichier */
            fp = (arch.pcurrent)->getCurrentDir();
    }

    beg_p = p;
    while (*beg_p == '/')
        ++beg_p;
    end_p = beg_p + 1;

    while (*beg_p != 0)
    {
        if (fp->getType() != TYPE_DIRECTORY) // 如果前面一个不是目录，则表示出错了
            return NULL;

        while (*end_p != 0 && *end_p != '/')
            ++end_p; 

        // 获取当前beg_p和end_p中间的文件
        name = (char *)kmalloc(end_p - beg_p + 1);
        memcpy(name, beg_p, end_p - beg_p);
        name[end_p - beg_p] = 0;

        
        if (strcmp(name, "..") == 0)
        {
            fp = fp->getParent();
        }
        else if (strcmp(name, ".") == 0)
        {
        }
        else
        {
            ofp = fp;

			if (fp->getType() == TYPE_LINK && (fp->getLink() != NULL))
			{
				fp = fp->getLink();
			}

			if (!(fp = fp->find(name)))//fd赋值为了当前文件
			{
				strcpy(fname, name);
				kfree(name);
				return ofp;
			}
        }
        beg_p = end_p;
        while (*beg_p == '/')
            ++beg_p;
        end_p = beg_p + 1;
        kfree(name);
    }
    return fp->getType() == TYPE_DIRECTORY? fp : fp->getParent();
}

// 将文件系统的根目录切换到 targetdir 指定的新目录。
u32 Filesystem::link(char *fname, char *newf)
{
    File *tolink = path(fname); // 找到路径下的文件
    if (tolink == NULL)
        return -1;

    char *nname = (char *)kmalloc(255);
    File *par = path_parent(newf, nname); // 找到父路径以及该路径指向的文件
    File *fp = new File(nname, TYPE_LINK);
    fp->setLink(tolink);
    par->addChild(fp);
    return RETURN_OK;
}

u32 Filesystem::addFile(char *dir, File *fp)
{
    File *fdir = path(dir);
    if (fdir == NULL)
        return ERROR_PARAM;
    else
    {
        return fdir->addChild(fp);
    }
}
