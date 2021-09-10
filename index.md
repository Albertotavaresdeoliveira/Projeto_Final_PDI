# Projeto Final da disciplina de PDI

## Alberto Tavares de Oliveira

Projeto Final da disciplina de Processamento Digital de Imagens da UFRN desenvolvido por Alberto Tavares de Oliveira.

### Descrição do projeto

Este projeto consiste na detecção de objetos em uma cena usando comparação de histogramas. As cenas podem ser os frames capturados de uma câmera, e neste caso, a detecção é feita em tempo real. O objeto a ser buscado é destacado na cena maior.

Abaixo é mostrado o código em c++ desenvolvido

```c++
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

Point Findimage(Mat scene, Mat img){
  Point p(150, 150);
  Mat scenetmp;

  Mat histscene[3];//Histogramas RGB da cena de busca
  Mat histimg[3];//Histogramas RGB da imagem buscada
  Mat sceneplanes[3];
  Mat imgplanes[3];
  
  double Dif[3];
  double DifSoma;
  double MenorDif = 0;//Valor inicial alto

  int scenewidth = scene.cols;
  int sceneheight = scene.rows;
  int imgwidth = img.cols;
  int imgheight = img.rows;
  int radius = (sqrt( imgwidth*imgheight + imgheight*imgheight )) / 2;
  
  cout <<"Dimenções da cena : (" << scenewidth << "," << sceneheight << ") " << endl;
  cout <<"Dimenções da imagem : (" << imgwidth << "," << imgheight << ") " << endl;

  int nbins = 64;
  float range[] = {0, 255};
  const float *histrange = { range };
  bool uniform = true;
  bool acummulate = false;

  int Ndivs = 3;

  int Numwidth = (scenewidth/(imgwidth/Ndivs));// * Ndivs;
  int Numheight = (sceneheight/(imgheight/Ndivs));// * Ndivs;
  
  Rect R;

  for(int i=0; i<Numheight-Ndivs+1; i++){
    for(int j=0; j<Numwidth-Ndivs+1; j++){

      //scenetmp = scene(Rect(x, y, imgwidth, imgheight));//Pedaço da cena com o tamanho da imagem buscada
      scenetmp = scene(Rect(j*(imgwidth/Ndivs), i*(imgheight/Ndivs), imgwidth, imgheight));
      split (scenetmp, sceneplanes);
      split (img, imgplanes);

      DifSoma = 0;
      
      for(int z=0; z<3; z++) { 
        calcHist(&sceneplanes[z], 1, 0, Mat(), histscene[z], 1, &nbins, &histrange, uniform, acummulate);
        calcHist(&imgplanes[z], 1, 0, Mat(), histimg[z], 1, &nbins, &histrange, uniform, acummulate); 
      }

      //for(int d=0; d<3; d++) { Dif[d] = compareHist(histscene[d], histimg[d], HISTCMP_CORREL); }
      for(int d=0; d<3; d++) { Dif[d] = compareHist(histscene[d], histimg[d], HISTCMP_INTERSECT); }
  
      for(int s=0; s<3; s++) { DifSoma = DifSoma + Dif[s]; }

      if(DifSoma >= MenorDif) { MenorDif = DifSoma; p.y = (i+1)*(imgheight/Ndivs); p.x = (j+1)*(imgwidth/Ndivs);}
      
    }
  }

  circle(scene, p, radius, Scalar(0, 0, 255), 5, LINE_8 );

  imshow("cena de busca", scene);
  imshow("imagem buscada", img);

  return p;
}

Point M, N; // Pontos para selecionar a imagem buscada na cena
Mat scene;
bool Clicked = false;

void locator(int event, int x, int y, int flags, void* userdata){ //function to track mouse movement and click//
   if (event == EVENT_LBUTTONDOWN){ //when left button clicked//
      cout << "Left click has been made, Position:(" << x << "," << y << ")" << endl;
      M.x = x; M.y = y;
      circle(scene, M, 5, Scalar(0, 0, 255), 3, LINE_8 );
      Clicked = true;
      imshow("cena", scene);
   } else if (event == EVENT_RBUTTONDOWN){ //when right button clicked//
      cout << "Rightclick has been made, Position:(" << x << "," << y << ")" << endl;
   } else if (event == EVENT_MBUTTONDOWN){ //when middle button clicked//
      cout << "Middleclick has been made, Position:(" << x << "," << y << ")" << endl;
   } else if (event == EVENT_MOUSEMOVE){ //when mouse pointer moves//
      cout << "Current mouse position:(" << x << "," << y << ")" << endl;
      N.x = x; N.y = y;
      //if(Clicked) { rectangle(scene, M, N, Scalar(0, 0, 255), 2, LINE_8, 0); }
   } else if (event == EVENT_LBUTTONUP){ //when left button is released
      cout << "Left click up has been made, Position:(" << x << "," << y << ")" << endl;
      rectangle(scene, M, N, Scalar(0, 0, 255), 2, LINE_8, 0);
      circle(scene, M, 5, Scalar(0, 0, 255), 3, LINE_8 );
      imshow("cena", scene);
   }
}

int main(int argc, char** argv){
  Mat img;
  Point P;
  VideoCapture cap;
  int key;

  namedWindow("cena");
  setMouseCallback("cena", locator, NULL);

	cap.open(0);

  if(!cap.isOpened()){ cout << "camera indisponivel"; return -1; }

  cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);//640
  cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);//480
  
  cout << "contrast :" << cap.get(CAP_PROP_CONTRAST) << endl;
  cout << "brilho :" << cap.get(CAP_PROP_BRIGHTNESS) << endl;
  cout << "saturation :" << cap.get(CAP_PROP_SATURATION) << endl;
  
  //img = imread(argv[1], IMREAD_COLOR);

  while(1){
    cap >> scene;

    //scene = imread(argv[1], IMREAD_COLOR);
    //img = imread(argv[2], IMREAD_COLOR);

    //P = Findimage(scene, img);
    imshow("cena", scene);
    key = waitKey(30);
    if(key == 27) break;
  }

  waitKey();

  scene(Rect(M, N)).copyTo(img);
  imshow("img", img);

  waitKey();

  while(1){
    cap >> scene;

    //scene = imread(argv[1], IMREAD_COLOR);
    //img = imread(argv[2], IMREAD_COLOR);

    P = Findimage(scene, img);
    imshow("cena", scene);
    key = waitKey(30);
    if(key == 27) break;
  }

  cout <<"Ponto (" << P.x << "," << P.y << ") é o centro" << endl;

  destroyAllWindows();
  //waitKey();

  return 0;
}
```

For more details see [GitHub Flavored Markdown](https://guides.github.com/features/mastering-markdown/).

### Jekyll Themes

Your Pages site will use the layout and styles from the Jekyll theme you have selected in your [repository settings](https://github.com/Albertotavaresdeoliveira/Projeto_Final_PDI/settings/pages). The name of this theme is saved in the Jekyll `_config.yml` configuration file.

### Support or Contact

Having trouble with Pages? Check out our [documentation](https://docs.github.com/categories/github-pages-basics/) or [contact support](https://support.github.com/contact) and we’ll help you sort it out.
