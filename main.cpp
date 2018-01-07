#include <iostream>
#include <stdio.h>
#include <vector>

#define plymax(a,b) a>b?a:b
#define plymin(a,b) a>b?b:a


struct Rect3D{
    double xmin;
    double xmax;
    double ymin;
    double ymax;
    double zmin;
    double zmax;

};

/**
 * 获取点云数据集的范围
 * @param pathPly
 * @param header
 * @return
 */
Rect3D ReadPlyPointsRange(const char* pathPly,int header,double &cx,double &cy,double &cz);

/**
 * 点云数据归一化到０－１输出
 * @param inPly
 * @param header
 * @param range
 * @param outPly
 * @return
 */
bool PlyPointsNormalize(const char* inPly,int header,double centerx,double centery,double centerz,Rect3D range,const char* outPly);

/**
 * 统一进行比例变化和平移
 * @param inPlys
 * @param outPlys
 * @return
 */
bool PlyTransformation(std::vector<std::string> inPlys,std::vector<std::string> outPlys,int headers);

int main() {
    std::vector<std::string> inPlys,outPlys;
    inPlys.push_back("/home/wuwei/Data/LAS/26.ply");
    inPlys.push_back("/home/wuwei/Data/LAS/21.ply");

    outPlys.push_back("/home/wuwei/Data/LAS/26Trans.ply");
    outPlys.push_back("/home/wuwei/Data/LAS/21Trans.ply");
    PlyTransformation(inPlys,outPlys,14);


    return 0;
}

Rect3D ReadPlyPointsRange(const char* pathPly,int header,double &cx,double &cy,double &cz)
{
    FILE* fply=fopen(pathPly,"r+");
    char line[2048];
    for(int i=0;i<header;++i)
        fgets(line,2048,fply);
    Rect3D range;
    range.xmax=range.ymax=range.zmax = -999999;
    range.xmin=range.ymin=range.zmin =  999999;
    int i=0;
    cx=cy=cz=0;

    while(!feof(fply)){
        fgets(line,2048,fply);
        double x,y,z;
        sscanf(line,"%lf %lf %lf",&x,&y,&z);

        cx+=x;
        cy+=y;
        cz+=z;

        range.xmax = plymax(x,range.xmax);
        range.ymax = plymax(y,range.ymax);
        range.zmax = plymax(z,range.zmax);

        range.xmin = plymin(x,range.xmin);
        range.ymin = plymin(y,range.ymin);
        range.zmin = plymin(z,range.zmin);

        ++i;
    }
    cx/=i;
    cy/=i;
    cz/=i;
    fclose(fply);
    return range;
}

bool PlyPointsNormalize(const char* inPly,int header,double centerx,double centery,double centerz,Rect3D range,const char* outPly)
{
    FILE* fplyIn =fopen(inPly,"r+");
    FILE* fplyOut=fopen(outPly,"w+");

    char line[2048];
    for(int i=0;i<header;++i){
        fgets(line,2048,fplyIn);
        fprintf(fplyOut,"%s",line);
    }

    double ratez = 1.0/(range.zmax-range.zmin);
    double trate = (range.xmax-range.xmin)/(range.zmax-range.zmin);
    double ratex = trate/(range.xmax-range.xmin);
    trate = (range.ymax-range.ymin)/(range.zmax-range.zmin);
    double ratey = trate/(range.ymax-range.ymin);

    double bias  = range.zmin;
    double biasx = centerx;
    double biasy = centery;

    double maxz =-999999;
    double minz =999999;

    int i=0;
    while(!feof(fplyIn)){
        fgets(line,2048,fplyIn);
        double x,y,z;
        int r,g,b,alpha;
        sscanf(line,"%lf %lf %lf %d %d %d %d",&x,&y,&z,&r,&g,&b,&alpha);
        z=(z-bias)*ratez;
        x=(x-biasx)*ratex;
        y=(y-biasy)*ratey;
        fprintf(fplyOut,"%lf %lf %lf %d %d %d %d\n",x,y,z,r,g,b,alpha);
        ++i;
    };
    fclose(fplyIn);
    fclose(fplyOut);
    return true;
}

bool PlyTransformation(std::vector<std::string> inPlys,std::vector<std::string> outPlys,int headers)
{
    std::vector<Rect3D> rects;
    for(int i=0;i<inPlys.size();++i)
    {
        double centerx = 0;
        double centery = 0;
        double centerz = 0;
        rects.push_back(ReadPlyPointsRange(inPlys[i].c_str(),headers,centerx,centery,centerz));
        PlyPointsNormalize(inPlys[i].c_str(),headers,centerx,centery,centerz,rects[i],outPlys[i].c_str());
    }
    return true;
}