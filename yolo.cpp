#include "yolo.h"


YOLO::YOLO(float confThreshold, float nmsThreshold, int inpW, int inpH, string classFile,
           string modelCfg, string modelWeights, string name)
{
//    cout << "Net use " << config.netname << endl;
//    this->confThreshold = config.confThreshold;
//    this->nmsThreshold = config.nmsThreshold;
//    this->inpWidth = config.inpWidth;
//    this->inpHeight = config.inpHeight;
//    strcpy(this->netname, config.netname.c_str());

//    ifstream ifs(config.classesFile.c_str());
//    string line;
//    while (getline(ifs, line)) this->classes.push_back(line);

//    this->net = readNetFromDarknet(config.modelConfiguration, config.modelWeights);
//    this->net.setPreferableBackend(DNN_BACKEND_OPENCV);
//    this->net.setPreferableTarget(DNN_TARGET_CPU);


//    config.netname = "yolov3";
//    config.modelWeights = "/home/hatice/yolov4/yolov34-cpp-opencv-dnn-master/yolov3/yolov3.weights";
//    config.modelConfiguration = "/home/hatice/yolov4/yolov34-cpp-opencv-dnn-master/yolov3/yolov3.cfg";
//    config.classesFile = "/home/hatice/yolov4/yolov34-cpp-opencv-dnn-master/coco.names";
//    config.inpHeight = 416;
//    config.inpWidth = 416;

//    config.netname = "yolov4";
//    config.modelWeights = "/home/hatice/yolov4/yolov34-cpp-opencv-dnn-master/yolov4/yolov4.weights";
//    config.modelConfiguration = "/home/hatice/yolov4/yolov34-cpp-opencv-dnn-master/yolov4/yolov4.cfg";
//    config.classesFile = "/home/hatice/yolov4/yolov34-cpp-opencv-dnn-master/coco.names";
//    config.inpHeight = 416;
//    config.inpWidth = 416;
    config.netname = name;
    config.modelWeights = modelWeights;
    config.modelConfiguration = modelCfg;
    config.classesFile = classFile;
    config.inpHeight = inpH;
    config.inpWidth = inpW;
    config.nmsThreshold = nmsThreshold;
    config.confThreshold = confThreshold;

    cout << "Net use " << config.netname << endl;
    this->confThreshold = config.confThreshold;
    this->nmsThreshold = config.nmsThreshold;
    this->inpWidth = config.inpWidth;
    this->inpHeight = config.inpHeight;
    strcpy(this->netname, config.netname.c_str());

    ifstream ifs(config.classesFile.c_str());
    string line;
    while (getline(ifs, line)) this->classes.push_back(line);

    this->net = readNetFromDarknet(config.modelConfiguration, config.modelWeights);
    this->net.setPreferableBackend(DNN_BACKEND_CUDA);
    this->net.setPreferableTarget(DNN_TARGET_CUDA);
}

void YOLO::postprocess(Mat& frame, const vector<Mat>& outs)   // Remove the bounding boxes with low confidence using non-maxima suppression
{
    vector<int> classIds;
    vector<float> confidences;
    vector<Rect> boxes;

    for (size_t i = 0; i < outs.size(); ++i)
    {
        // Scan through all the bounding boxes output from the network and keep only the
        // ones with high confidence scores. Assign the box's class label as the class
        // with the highest score for the box.
        float* data = (float*)outs[i].data;
        for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols)
        {
            Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
            Point classIdPoint;
            double confidence;
            // Get the value and location of the maximum score
            minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
            if (confidence > this->confThreshold)
            {
                int centerX = (int)(data[0] * frame.cols);
                int centerY = (int)(data[1] * frame.rows);
                int width = (int)(data[2] * frame.cols);
                int height = (int)(data[3] * frame.rows);
                int left = centerX - width / 2;
                int top = centerY - height / 2;

                classIds.push_back(classIdPoint.x);
                confidences.push_back((float)confidence);
                boxes.push_back(Rect(left, top, width, height));
            }
        }
    }


    vector<int> indices;
    NMSBoxes(boxes, confidences, this->confThreshold, this->nmsThreshold, indices);

    for (size_t i = 0; i < indices.size(); ++i)
    {
        int idx = indices[i];
        Rect box = boxes[idx];

        if(this->classes[classIds[idx]] == yolo_objects && confidences[idx] > 0.5)
        {

            Rect _rect;
            _rect.x = box.x;
            _rect.y = box.y;
            _rect.width = box.width;
            _rect.height = box.height;
            predbox.push_back(_rect);
            this->drawPred(classIds[idx], confidences[idx], box.x, box.y,
                box.x + box.width, box.y + box.height, frame);
        }


    }
}

void YOLO::drawPred(int classId, float conf, int left, int top, int right, int bottom, Mat& frame)   // Draw the predicted bounding box
{
    //Draw a rectangle displaying the bounding box
    rectangle(frame, Point(left, top), Point(right, bottom), Scalar(0, 0, 255), 3);

    //Get the label for the class name and its confidence
    string label = format("%.2f", conf);
    if (!this->classes.empty())
    {
        CV_Assert(classId < (int)this->classes.size());
        label = this->classes[classId] + ":" + label;
    }

    //Display the label at the top of the bounding box
    int baseLine;
    Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
    top = max(top, labelSize.height);
    rectangle(frame, Point(left, top - int(1.5 * labelSize.height)), Point(left + int(1.5 * labelSize.width), top + baseLine), Scalar(0, 255, 0), FILLED);
    putText(frame, label, Point(left, top), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(255, 0, 0), 2);
}

void YOLO::detect(Mat& frame)
{
    Mat blob;
    blobFromImage(frame, blob, 1 / 255.0, Size(this->inpWidth, this->inpHeight), Scalar(0, 0, 0), true, false);
    this->net.setInput(blob);
    vector<Mat> outs;
    this->net.forward(outs, this->net.getUnconnectedOutLayersNames());
    this->postprocess(frame, outs);

    vector<double> layersTimes;
    double freq = getTickFrequency() / 1000;
    double t = net.getPerfProfile(layersTimes) / freq;
    string label = format("%s Inference time : %.2f ms", this->netname, t);
    putText(frame, label, Point(0, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);
//    imwrite(format("%s_out.jpg", this->netname), frame);
}
