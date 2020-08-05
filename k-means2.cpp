#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <limits>
using namespace std;
float los = 0.000001;   //���������ж���������Χ
 //�����������
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

void k_means(vector<point>& dataset, vector<point>& cluster,int k); //k-means�����㷢���岿��
vector<point> openFile(const char* filename);   //���ļ��ж�ȡ�㼯
float squareDistance(point a, point b);   //��������֮���ŷʽ���루ֱ�߾��룩
void show(vector<point> dataset, vector<point> cluster,int k);  //��ʾ�ִغ��ͼ�ν��
void writefile(const char* filename, vector<point> dataset, vector<point> cluster);

int main(int argc, char** argv) 
{
	vector<point> dataset = openFile("k-means_input.txt");
	vector<point> cluster;
	k_means(dataset, cluster, 4);
	writefile("k-means_output.txt",dataset, cluster);
	show(dataset, cluster, 4);  //ͼ��ģ������ִؽ��
	return 0;
}


//���ı��еĵ���뵽vector����
vector<point> openFile(const char* filename)
{
	fstream file(filename, ios::in);
	vector<point> data;
	int x, y;

	while (!file.eof())  //�ļ�δ���������ѭ��
	{
		file >> x >> y;
		point p(x, y, 0);    //������Ϊ0��ʾ��ʼʱ����δ���зִ�
		data.push_back(p);
	}
	file.close();
	return data;
}

//��������֮��ľ���
float squareDistance(point a, point b)
{
	return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
}

//k-means����
void k_means(vector<point>& dataset, vector<point>& centroid, int k) //k-means�����㷢���岿��
{
	//vector<point> centroid;    //����һ��vector��������ž������ĵ�
	int n = 1;
	int len = dataset.size();  //�������ݼ����ж��ٸ���
	srand((int)time(0));

	for (int i = 1; i <= k; i++)
	{
		int index = rand() % len;  //�������һ����ֵ��Ϊdataset������±�
		point Init_centroid(dataset[index].x, dataset[index].y, i);  //ȡ��dataset�������±�Ϊcen��Ӧ�ĵ���Ϊ�������ĵ�,i��ʾ���ڵ�i����
		centroid.push_back(Init_centroid);   //���������ĵ�Init_centroid�����ž������ĵ��vector����centroid
	}

	//���ÿ���������ĵ�����꼰������ʾ�Ĵ�
	cout << "��ʼ��������:" << endl;
	for (int i = 0; i < k; i++)
	{
		cout << "cluster " << i << " : ( " << centroid[i].x << ", " << centroid[i].y << " )" << endl;
	}
	cout << endl;

	int flag = 1;   //����������־λ������㷨��������flag��0  
	//�������whileѭ�����е�����⣬ǰ��Ĳ���Ϊ��ʼ����������
	int num = 0;   //�����������
	while (flag)   //��flag=1����˵���㷨δ�����������������������ѭ�����ó�������
	{
		num++;   //�ܹ��ĵ�������
		for (int i = 0; i < len; i++)  //�����е���зִ�
		{
			n = 1;   							//n��ʾ��n����������
			float shortest = INT_MAX;     	//����㵽�������ĵľ��벢�����ʼ��Ϊ���ֵ
			int cur = dataset[i].cluster; 	//�����i�ľ������ģ�����������µ�i�ľ�������
			while (n <= k)  					//���μ����i��ÿ���������ĵľ��룬����鵽���Լ�������Ǹ���������
			{
				float temp = squareDistance(dataset[i], centroid[n - 1]);		 //�����i���㵽��n���������ĵ�֮��ľ���	
				if (temp < shortest)   //�ѵ�鵽���Լ�������Ǹ���������
				{
					shortest = temp;
					cur = n;
				}
				n++;
			}
			dataset[i].cluster = cur;   //���µ�i�����Ĵ�
		}

		//���´صľ������ĵ�����
		vector<point> new_centroid;   //���ڴ���µľ�������
		int* cs = new int[k];    //���ڴ��ÿ�����к��е�ĸ���
		for (int i = 1; i <= k; i++)  //��ʼ���¾������ĵ�����Ϊ0,����ʼ��cs[i]=0
		{
			point temp(0, 0, i);
			new_centroid.push_back(temp);
			cs[i - 1] = 0;
		}

		for (int i = 0; i < len; i++)  //��ÿ���������е������ֵ�������
		{
			new_centroid[dataset[i].cluster - 1].x += dataset[i].x;  //��ÿ�����е����е������ֵ������Ͳ���
			new_centroid[dataset[i].cluster - 1].y += dataset[i].y;
			cs[dataset[i].cluster - 1]++;    //ͨ���ۼ����ÿ���������е�ĸ���
		}

		for (int i = 0; i < k; i++)
		{
			new_centroid[i].x = new_centroid[i].x / cs[i];     //ȡÿ���������е��ƽ��ֵ��Ϊ�µľ�������
			new_centroid[i].y = new_centroid[i].y / cs[i];
		}

		flag = 0;
		for (int i = 0; i < k; i++)
		{
			if (squareDistance(new_centroid[i], centroid[i]) > los) //����µľ���������ɵľ������ĵľ����ڲ�����Χ�ڣ����¾ɲ�ͬ������flag��1
				flag = 1;
			centroid[i] = new_centroid[i];
		}
	}
	cout << "�ִغ��������:" << endl;
	for (int i = 0; i < k; i++)
	{
		cout << "cluster "<<i<<" : ( " << centroid[i].x << ", " << centroid[i].y <<" )"<< endl;
	}
	cout << endl;
	cout << "��������:"<<num << endl;
}

void writefile(const char* filename, vector<point> dataset, vector<point> cluster)
{
	int len = dataset.size();  //�������ݼ����ж��ٸ���
	fstream file_out(filename, ios::out);

	file_out << "cluster 1:"<<"( "<< cluster[0].x<<", "<< cluster[0].y<<" )"<<endl;
	for (int i = 0,k=0; i < len; i++)
	{
		if (k == 5)   //����ÿ��������ݵĸ���
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
		if (k == 5)   //����ÿ��������ݵĸ���
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
		if (k == 5)   //����ÿ��������ݵĸ���
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
		if (k == 5)   //����ÿ��������ݵĸ���
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

void show(vector<point> dataset, vector<point> cluster, int k)  //��ʾ�ִغ��ͼ�ν��
{
	char a[30][30];   //����ģ����ʾ�ִؽ��
	int len = dataset.size();  //�������ݼ����ж��ٸ���
	for (int i = 0; i < 30; i++)  //��ʼ��Ϊ�ո��ַ�����ʾ������
	{
		for (int j = 0; j < 30; j++)
		{
			a[i][j] = ' ';
		}
	}

	for (int i = 0; i < len; i++)  
	{
		a[int(dataset[i].x)][int(dataset[i].y)] = 'o';  //��o��ʾ������λ���е�
	}
	cout << "δ�ִ�ʱ:" << endl;
	for (int i = 0; i < 30; i++)  //��ʾδ�ִ�ʱ�ĵ㼯
	{
		for (int j = 0; j < 30; j++)
		{
			cout << a[i][j];
		}
		cout << endl;
	}
	cout << endl;

	for (int i = 0; i < len; i++)  //�ò�ͬ���ַ�ģ�ⲻͬ������
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
		a[(int)(cluster[i].x+0.5)][(int)(cluster[i].y + 0.5)] = 'O';   //��������ȡ��
	}
	cout << "�ִغ�" << endl;
	for (int i = 0; i < 30; i++)  //��ʾ�ִغ��Ч��
	{
		for (int j = 0; j < 30; j++)
		{
			cout << a[i][j];
		}
		cout << endl;
	}
}
