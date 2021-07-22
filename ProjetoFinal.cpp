#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char** argv){
  Mat image, escalacinza, histAnterior, histNovo;
  int width, height;
  VideoCapture cap;
  int nbins = 64;
  float range[] = {0, 255};
  const float *histrange = { range };
  bool uniform = true;
  bool acummulate = false;
  int key;

  cap.open(0);
  if(!cap.isOpened()){ cout << "cameras indisponiveis"; return -1; }

  cap.set(CAP_PROP_FRAME_WIDTH, 640);
  cap.set(CAP_PROP_FRAME_HEIGHT, 480);
  width = cap.get(CAP_PROP_FRAME_WIDTH);
  height = cap.get(CAP_PROP_FRAME_HEIGHT);

  Point Centro(width/2, height/2);

  cout << "largura = " << width << endl;
  cout << "altura  = " << height << endl;

  int histw = nbins, histh = nbins/2;
  Mat histImgAnterior(histh, histw, CV_8UC1, Scalar(0));
  Mat histImgNovo(histh, histw, CV_8UC1, Scalar(0));

  double Dif = 0, Limiar = 0.999;
  
  cap >> image;
  cvtColor(image,escalacinza, COLOR_RGB2GRAY);
  calcHist(&escalacinza, 1, 0, Mat(), histAnterior, 1, &nbins, &histrange, uniform, acummulate);
  normalize(histAnterior, histAnterior, 0, histImgAnterior.rows, NORM_MINMAX, -1, Mat());

  while(1){
    cap >> image;
    cvtColor(image,escalacinza, COLOR_RGB2GRAY);
    calcHist(&escalacinza, 1, 0, Mat(), histNovo, 1, &nbins, &histrange, uniform, acummulate);
    normalize(histNovo, histNovo, 0, histImgNovo.rows, NORM_MINMAX, -1, Mat());

    histImgNovo.setTo(Scalar(0));

    Dif = compareHist(histAnterior, histNovo, HISTCMP_CORREL);
    //Dif = compareHist(histAnterior, histNovo, HISTCMP_CHISQR);
    //Dif = compareHist(histAnterior, histNovo, HISTCMP_INTERSECT);
    //Dif = compareHist(histAnterior, histNovo, HISTCMP_BHATTACHARYYA);
    //Dif = compareHist(histAnterior, histNovo, HISTCMP_HELLINGER );

    if(Dif<=Limiar) { circle(escalacinza, Centro, 50, Scalar(255, 255, 255), FILLED, LINE_8 ); }

    for(int i=0; i<nbins; i++){
      line(histImgNovo, Point(i, histh), Point(i, histh-cvRound(histNovo.at<float>(i))), Scalar(255, 255, 255), 1, 8, 0);
    }

    histImgNovo.copyTo(escalacinza(Rect(0, 0, nbins, histh)));
    
    histAnterior = histNovo.clone();

    imshow("image", escalacinza);
    key = waitKey(30);
    if(key == 27) break;
  }
  return 0;
}