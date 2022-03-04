
#define _CRT_SECURE_NO_WARNINGS // localTimeの警告避け

#ifdef _DEBUG
#pragma comment(lib, "opencv_world453d.lib")
#else
#pragma comment(lib, "opencv_world453.lib")
#endif

#include <opencv2/opencv.hpp>
#include <Windows.h>
#include <iostream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <math.h>
#include <vector>
#include <filesystem>


using namespace std;

int camera_id = 0;
int camera_width = 640;
int camera_height = 480;
double camera_fps = 30;


int LoadConfigFile(const char* filename)
{
	ifstream fin(filename);
	if (!fin) {
		cout << filename << " can not open.\n";
		return -1;
	}
	string word;
	while (fin >> word) {
		if (word == "[CameraID]") {
			fin >> camera_id;
		}
		else if (word == "[ImageSize]") {
			fin >> camera_width >> camera_height;
		}
		else if (word == "[FPS]") {
			fin >> camera_fps;
		}
	}
	fin.close();
	return 0;
}


string getDatetimeStr() {
    time_t t = time(nullptr);
    const tm* localTime = localtime(&t);
    std::stringstream s;
    s << localTime->tm_year + 1900 << "年";
    // setw(),setfill()で0詰め
    s << setw(2) << setfill('0') << localTime->tm_mon + 1 << "月"; 
    s << setw(2) << setfill('0') << localTime->tm_mday << "日";
    s << setw(2) << setfill('0') << localTime->tm_hour << "時";
    s << setw(2) << setfill('0') << localTime->tm_min << "分";
    s << setw(2) << setfill('0') << localTime->tm_sec << "秒";
    // std::stringにして値を返す
    return s.str();
}


int main()
{
    // 設定ファイル読み込み
    LoadConfigFile("config.txt");

    // 0未満なら入力を受け付ける
    if (camera_id < 0) {
        cout << "使用するカメラの番号（0～）を入力してください：";
        cin >> camera_id;
        cout << endl;
    }

    cv::VideoCapture cap;
    cap.open(camera_id);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, camera_width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, camera_height);
    cap.set(cv::CAP_PROP_FPS, camera_fps);

    if (!cap.isOpened()) {
        return -1;
    }

    cout << "設定しようとした情報（※反映されるとは限らない）" << endl;
    cout << "[CameraID] " << camera_id << endl;
    cout << "[ImageSize] " << camera_width << " " << camera_height << endl;
    cout << "[FPS] " << camera_fps << endl;
    cout << endl;

    // double fps = cap.get(cv::CAP_PROP_FPS); // ゼロになる
    int w = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int h = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    cout << "実際の画像サイズ: " << w << "x" << h << endl << endl;

    cv::Mat frame;
    int k = 0;
    string dirname;

    cout << "《画像表示ウィンドウがアクティブな状態での操作》" << endl;
    cout << "SPACE : 画像を保存" << endl;
    cout << "ESC   : 終了" << endl;

    while (true) {
        cap.read(frame);
        cv::imshow("capture", frame);
        const int key = cv::waitKey(1);

        if (key == ' ') {
            if (k == 0) {
                // 時刻名のフォルダを作成
                dirname = getDatetimeStr();
                cout << endl << dirname << " というフォルダに撮影画像を格納します。" << endl;
                std::filesystem::create_directory(dirname);
            }
            string filename;
            stringstream ss;
            ss << dirname << "\\"; // 保存先フォルダ
            ss << setfill('0') << setw(4) << right << to_string(k) << ".jpg";
            ss >> filename;
            cv::imwrite(filename, frame);
            k++;
        }
        else if (key == 'q' || key==27 ) { // q or ESC
            break;
        }
    }
    cv::destroyAllWindows();

	return 0;
}