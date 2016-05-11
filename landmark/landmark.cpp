#include <dlib/opencv.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>

using namespace dlib;
using namespace std;
using namespace cv;

/*通过像素信息筛选人脸*/
bool detectByHist(Mat input){
	Mat temp;
	cvtColor(input,temp,CV_BGR2GRAY);
	int count=0;
	for (int i=0;i<temp.rows;i++)
	{
		uchar* data= temp.ptr<uchar>(i);
		for (int j=0;j<temp.cols;j++)
		{
			if(data[j]<50||data[j]>200){
				count++;
			}
		}
	}
	double x=(double)count/(temp.cols*temp.rows);
	cout<<x<<endl;
	double thre=0.2;
	if (x>thre)
	{
		cout<<"true"<<endl;
		return true;
	}else
	{
		cout<<"false"<<endl;
		return false;
	}
	 
}


int main()
{
    try
    {

        cv::VideoCapture cap("C:/Users/zhao/Desktop/2016.3.12/2.avi");//参数为0代表读摄像头，这里可以改成读取文件
        image_window win;

        // 加载人脸分类器和特征点模型
        frontal_face_detector detector = get_frontal_face_detector();
        shape_predictor pose_model;
        deserialize("shape_predictor_68_face_landmarks.dat") >> pose_model;

        
        while(!win.is_closed())
        {
			// 读摄像头视频
            cv::Mat temp;
            cap >> temp;
				
			//将OpenCV数据格式转为DLib
            cv_image<bgr_pixel> cimg(temp);
			
			//计时
			double t = 0;
			t = (double)cvGetTickCount();
            // 人脸检测
            std::vector<dlib::rectangle> faces = detector(cimg);
			t = (double)cvGetTickCount() - t;
			printf( "detection time = %g ms\n", t/((double)cvGetTickFrequency()*1000.) );//输出人脸检测时间

            // 特征点标记
            std::vector<full_object_detection> shapes;
			//计时
			t = 0;
			t = (double)cvGetTickCount();

            for (unsigned long i = 0; i < faces.size(); ++i){
				
				cv:Rect r=cv::Rect((int)faces[i].left(),(int)faces[i].top(),(int)faces[i].width(),(int)faces[i].height());
				Mat roi=temp(r);
					//获取人脸区域
				//imwrite("1.jpg",roi);//检查是否成功找到人脸
				if(!detectByHist(temp)){
					continue;//判断是不是人脸
				}

				shapes.push_back(pose_model(cimg, faces[i]));
				}

			t = (double)cvGetTickCount() - t;
			printf( "alignment time = %g ms\n", t/((double)cvGetTickFrequency()*1000.) );//输出特征点标记时间
           
			// 在窗口显示
            win.clear_overlay();
            win.set_image(cimg);
            win.add_overlay(render_face_detections(shapes));//在图片上画出68个特征点连成的线条

			
			//眼睛区域画上了圆圈
			point pts;
			std::vector<image_window::overlay_circle> overlay_circles;
			const rgb_pixel color = rgb_pixel(255,0,0);
			int thre=8;
			for (unsigned long j=0;j<shapes.size();++j)
			{
					for (unsigned long m=36;m<48;++m)
					{	
						pts=shapes[j].part(m);	
						overlay_circles.push_back(image_window::overlay_circle(pts,3,color));	
					}
					win.add_overlay(overlay_circles);//显示眼睛的特征点标记
			//判断是否眨眼
					double d_a=shapes[j].part(41).y()-shapes[j].part(37).y();	
					double d_b=shapes[j].part(40).y()-shapes[j].part(38).y();	
					double d_c=shapes[j].part(47).y()-shapes[j].part(43).y();	
					double d_d=shapes[j].part(46).y()-shapes[j].part(44).y();	
					double width=shapes[j].part(45).x()-shapes[j].part(36).x();	
				
					if (d_a<thre&&d_b<thre&&d_c<thre&&d_d<thre&&width>100)
					{
					cout<<"blink.You are alive"<<endl;
					}
			///////////

			}	
			
    }

	}
    catch(serialization_error& e)
    {
        cout << "没有找到模型" << endl;
    }
    catch(exception& e)
    {
        cout << e.what() << endl;
    }
}

