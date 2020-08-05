#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <limits>
using namespace std;
float los = 0.000001;   //定义用于判断收敛的误差范围
 //定义坐标点类
class point
{
public:
	point() {}
	point(float a, float b, int c)
	{
		x = a;
		y = b;
		cluster = c;
	}
	float x;
	float y;
	int cluster;
};

void k_means(vector<point>& dataset, vector<point>& cluster,int k); //k-means分类算发主体部分
vector<point> openFile(const char* filename);   //从文件中读取点集
float squareDistance(point a, point b);   //计算两点之间的欧式距离（直线距离）
void show(vector<point> dataset, vector<point> cluster,int k);  //显示分簇后的图形结果
void writefile(const char* filename, vector<point> dataset, vector<point> cluster);

int main(int argc, char** argv) 
{
	vector<point> dataset = openFile("k-means_input.txt");
	vector<point> cluster;
	k_means(dataset, cluster, 4);
	writefile("k-means_output.txt",dataset, cluster);
	show(dataset, cluster, 4);  //图像化模拟输出分簇结果
	return 0;
}


//将文本中的点读入到vector容器
vector<point> openFile(const char* filename)
{
	fstream file(filename, ios::in);
	vector<point> data;
	int x, y;

	while (!file.eof())  //文件未读完则进入循环
	{
		file >> x >> y;
		point p(x, y, 0);    //所属簇为0表示初始时数据未进行分簇
		data.push_back(p);
	}
	file.close();
	return data;
}

//计算两点之间的距离
float squareDistance(point a, point b)
{
	return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
}

//k-means聚类
void k_means(vector<point>& dataset, vector<point>& centroid, int k) //k-means分类算发主体部分
{
	//vector<point> centroid;    //定义一个vector容器，存放聚类中心点
	int n = 1;
	int len = dataset.size();  //计算数据集共有多少个点
	srand((int)time(0));

	for (int i = 1; i <= k; i++)
	{
		int index = rand() % len;  //随机产生一个数值作为dataset数组的下标
		point Init_centroid(dataset[index].x, dataset[index].y, i);  //取出dataset数组中下标为cen对应的点作为聚类中心点,i表示属于第i个簇
		centroid.push_back(Init_centroid);   //将聚类中心点Init_centroid存入存放聚类中心点的vector容器centroid
	}

	//输出每个聚类中心点的坐标及其所表示的簇
	cout << "初始聚类中心:" << endl;
	for (int i = 0; i < k; i++)
	{
		cout << "cluster " << i << " : ( " << centroid[i].x << ", " << centroid[i].y << " )" << endl;
	}
	cout << endl;

	int flag = 1;   //用于收敛标志位，如果算法收敛，则将flag置0  
	//下面进入while循环进行迭代求解，前面的步骤为初始化聚类中心
	int num = 0;   //定义迭代次数
	while (flag)   //若flag=1，则说明算法未收敛，继续迭代，否则结束循环，得出分类结果
	{
		num++;   //总共的迭代次数
		for (int i = 0; i < len; i++)  //对所有点进行分簇
		{
			n = 1;   							//n表示第n个聚类中心
			float shortest = INT_MAX;     	//定义点到聚类中心的距离并将其初始化为最大值
			int cur = dataset[i].cluster; 	//定义点i的聚类中心，用于下面更新点i的聚类中心
			while (n <= k)  					//依次计算点i到每个聚类中心的距离，将其归到离自己最近的那个聚类中心
			{
				float temp = squareDistance(dataset[i], centroid[n - 1]);		 //计算第i个点到第n个聚类中心点之间的距离	
				if (temp < shortest)   //把点归到离自己最近的那个聚类中心
				{
					shortest = temp;
					cur = n;
				}
				n++;
			}
			dataset[i].cluster = cur;   //更新点i所属的簇
		}

		//更新簇的聚类中心点坐标
		vector<point> new_centroid;   //用于存放新的聚类中心
		int* cs = new int[k];    //用于存放每个簇中含有点的个数
		for (int i = 1; i <= k; i++)  //初始化新聚类中心的坐标为0,并初始化cs[i]=0
		{
			point temp(0, 0, i);
			new_centroid.push_back(temp);
			cs[i - 1] = 0;
		}

		for (int i = 0; i < len; i++)  //将每个簇中所有点的坐标值进行求和
		{
			new_centroid[dataset[i].cluster - 1].x += dataset[i].x;  //将每个簇中的所有点的坐标值进行求和操作
			new_centroid[dataset[i].cluster - 1].y += dataset[i].y;
			cs[dataset[i].cluster - 1]++;    //通过累加求出每个簇中所有点的个数
		}

		for (int i = 0; i < k; i++)
		{
			new_centroid[i].x = new_centroid[i].x / cs[i];     //取每个簇中所有点的平均值作为新的聚类中心
			new_centroid[i].y = new_centroid[i].y / cs[i];
		}

		flag = 0;
		for (int i = 0; i < k; i++)
		{
			if (squareDistance(new_centroid[i], centroid[i]) > los) //如果新的聚类中心与旧的聚类中心的距离在不在误差范围内（即新旧不同），则将flag置1
				flag = 1;
			centroid[i] = new_centroid[i];
		}
	}
	cout << "分簇后聚类中心:" << endl;
	for (int i = 0; i < k; i++)
	{
		cout << "cluster "<<i<<" : ( " << centroid[i].x << ", " << centroid[i].y <<" )"<< endl;
	}
	cout << endl;
	cout << "迭代次数:"<<num << endl;
}

void writefile(const char* filename, vector<point> dataset, vector<point> cluster)
{
	int len = dataset.size();  //计算数据集共有多少个点
	fstream file_out(filename, ios::out);

	file_out << "cluster 1:"<<"( "<< cluster[0].x<<", "<< cluster[0].y<<" )"<<endl;
	for (int i = 0,k=0; i < len; i++)
	{
		if (k == 5)   //控制每行输出数据的个数
		{
			k = 0;
			file_out << endl;
		}
		if (dataset[i].cluster == 1)
		{
			file_out << "( "<<dataset[i].x << ", " << dataset[i].y<<" )" <<" ";
			k++;
		}
	}
	file_out << endl;

	file_out << "cluster 2:" << "( " << cluster[1].x << ", " << cluster[1].y << " )" << endl;
	for (int i = 0,k=0; i < len; i++)
	{
		if (k == 5)   //控制每行输出数据的个数
		{
			k = 0;
			file_out << endl;
		}
		if (dataset[i].cluster == 2)
		{
			file_out << "( " << dataset[i].x << ", " << dataset[i].y << " )" << " ";
			k++;
		}
	}
	file_out << endl;

	file_out << "cluster 3:" << "( " << cluster[2].x << ", " << cluster[2].y << " )" << endl;
	for (int i = 0,k=0; i < len; i++)
	{
		if (k == 5)   //控制每行输出数据的个数
		{
			k = 0;
			file_out << endl;
		}
		if (dataset[i].cluster == 3)
		{
			file_out << "( " << dataset[i].x << ", " << dataset[i].y << " )" << " ";
			k++;
		}
	}
	file_out << endl;

	file_out << "cluster 4:" << "( " << cluster[3].x << ", " << cluster[3].y << " )" << endl;
	for (int i = 0,k=0; i < len; i++)
	{
		if (k == 5)   //控制每行输出数据的个数
		{
			k = 0;
			file_out << endl;
		}
		if (dataset[i].cluster == 4)
		{
			file_out << "( " << dataset[i].x << ", " << dataset[i].y << " )" << " ";
			k++;
		}
	}
}

void show(vector<point> dataset, vector<point> cluster, int k)  //显示分簇后的图形结果
{
	char a[30][30];   //用于模拟显示分簇结果
	int len = dataset.size();  //计算数据集共有多少个点
	for (int i = 0; i < 30; i++)  //初始化为空格字符，表示无数据
	{
		for (int j = 0; j < 30; j++)
		{
			a[i][j] = ' ';
		}
	}

	for (int i = 0; i < len; i++)  
	{
		a[int(dataset[i].x)][int(dataset[i].y)] = 'o';  //用o表示该坐标位置有点
	}
	cout << "未分簇时:" << endl;
	for (int i = 0; i < 30; i++)  //显示未分簇时的点集
	{
		for (int j = 0; j < 30; j++)
		{
			cout << a[i][j];
		}
		cout << endl;
	}
	cout << endl;

	for (int i = 0; i < len; i++)  //用不同的字符模拟不同的区域
	{
		if (dataset[i].cluster == 1)
		{
			a[int(dataset[i].x)][int(dataset[i].y)] = '*';  
		}
		if (dataset[i].cluster == 2)
		{
			a[int(dataset[i].x)][int(dataset[i].y)] = '#';
		}
		if (dataset[i].cluster == 3)
		{
			a[int(dataset[i].x)][int(dataset[i].y)] = '+';
		}
		if (dataset[i].cluster == 4)
		{
			a[int(dataset[i].x)][int(dataset[i].y)] = '.';
		}
	}

	cout << endl;
	for (int i = 0; i < k; i++)
	{
		a[(int)(cluster[i].x+0.5)][(int)(cluster[i].y + 0.5)] = 'O';   //四舍五入取整
	}
	cout << "分簇后：" << endl;
	for (int i = 0; i < 30; i++)  //显示分簇后的效果
	{
		for (int j = 0; j < 30; j++)
		{
			cout << a[i][j];
		}
		cout << endl;
	}
}
