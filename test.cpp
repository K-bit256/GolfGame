#include "DxLib.h"
#include <fstream>
#include "json.hpp"
#include <vector>
#include <string>
#include "tinyxml2.h"
#include <math.h>

using json = nlohmann::json;
using namespace tinyxml2;
struct Ball{
    float x = 0;//コースセレクトで上書きする←↓
    float y = 0;

    float vx;
    float vy;

    float v0 = 170;

    bool moving;
};
Ball ball;
struct PLAYER {
    float x = 0;//コースの進捗状況の把握用
    float y = 0;

    bool moving;
};
PLAYER player;
struct Frame {
    int x, y, w, h;
    int fx, fy, fw, fh; // frameX, frameY, frameWidth, frameHeight
};
struct Animation{
    int imageHandle;
    std::vector<Frame> frames;
    
    int drawX;
    int drawY;
    bool loop;
    int speed = 10;
};
struct FontFrame{
    int x;
    int y;
    int w;
    int h;
};

struct FontChar{
    int handle;

    FontFrame idle;
    FontFrame bounce;
};
std::map<std::string, FontChar> fontData;
std::unordered_map<std::string, int> loadedSheets;


Animation idleAnim;
Animation readyAnim;
Animation throwAnim;
Animation gameBGAnim0;
Animation gameBGAnim1;
//Animation AnimItem0;
Animation AnimItem1_1;
Animation AnimItem1_2;
Animation AnimItem2_1;
Animation AnimItem2_2;
Animation AnimItem3_1;
Animation AnimItem3_2;
Animation AnimItem4_1;
Animation AnimItem4_2;
Animation AnimItem5_1;
Animation AnimItem5_2;
Animation AnimItem6_1;
Animation AnimItem6_2;
Animation ExplaincatAnim;
Animation CheercatAnim;
Animation SplashAnim;
Animation TryAnim;
Animation WatercatAnim;

Animation* currentAnim = &idleAnim;//代入
Animation* BGAnim = &gameBGAnim0;//代入
Animation* currentAnim3 = &AnimItem1_2;//
Animation* currentAnim4 = &ExplaincatAnim;//猫系

//std::vector<Frame> animIdle;
//std::vector<Frame> animReady;
//std::vector<Frame> animThrow;

enum GameState{
    TITLE,
    MENU,
    OPTIONS,
    TEST,
    TEST2,
    GAMEPLAY
};
enum PlayState{
    COURSE_SELECT, // コース選択
    EXPLAIN,
    COURSE_PREVIEW,
    COURSE_PREVIEW_ZOOMOUT,
    COURSE_PREVIEW_ZOOMIN,
    AIMING,        // 角度選択
    WAIT_THROW,    // 投げ待ち
    THROW,
    THROWEND,//BALL_MOVING    // ボール移動中
    ANIME0,
    ANIME1,
    ANIME2,
    ANIME3,
    ANIME4,
    ANIME5,
    ANIME6,
    RESULT
};
PlayState playState = COURSE_SELECT;
GameState currentState = TITLE;
//char Key[256];
//char OldKey[256] = { 0 };
bool showInputMenu = true;
int waitTimer = 0;
int  course = 0;
int currentFrame = 0;
int frameTimer = 0;
int currentFrame2 = 0;
int frameTimer2 = 0;
int currentFrame3 = 0;
int frameTimer3 = 0;
int currentFrame4 = 0;
int frameTimer4 = 0;

float previewCameraX = 0;
float cameraTargetX = 0;
int previewTimer = 0;
float previewScale;
float courseLength;//コース全長
float centerX;//スタートとゴールの中間
float targetScale;//目標スケール
bool lakecount = false;
bool boundcount = true;
bool goalcount = false;
bool back = false;
float xbefor = 500;//x座標を記憶するための変数

int angles[] = { -45, 0, 45 };
int angleIndex = 1;
int shotAngle = angles[angleIndex];
float g = 9.8;  //　重力加速度
int i = 1;
int coursenumber = 0;//どのコースを選んだか
int space_cnt = 0;
int throw_cnt = 0;

int gameBG;
int hederImg;
int easyImg;
int normalImg;
int hardImg;
int easySelectImg;
int normalSelectImg;
int hardSelectImg;

int miniHumanImg; // �l
int miniFlagImg;  // ��
int miniWallImg;
int miniWaterImg;
//int WallImg;
//int WaterImg;
//int GoalImg;
//int StartImg;
//int GroundImg;
int GroundCoverImg;//不要
int TemaeImg;
int HaikeiImg;
int BallImg;
int bgmHandle;
int fontSheetHandle;

int Explain1Img;
int Explain2Img;
int Explain3Img;
int Explain4Img;

int angleImg;
int angle0Img;
int angle45Img;
int angleMinus45Img;

struct StartData{
    int x, y;
    int width;
    int height;
};

struct GoalData{
    int x, y;
    int radius;
    int width;
    int height;
};

struct ObstacleData{
    std::string type;
    int x;
    int y;
    int width;
    int height;
    float angle;
};
struct GroundData {
    int y;
    int height;
};
struct CourseImages {
    int backgroundImg = -1;
    int startImg = -1;
    int goalImg = -1;
    int groundImg = -1;
    int wallImg = -1;
    int waterImg = -1;
};
struct CourseData {
    StartData start;
    GoalData goal;
    GroundData ground;

    CourseImages images;

    std::vector<ObstacleData> obstacles;
};
CourseData currentCourse;
CourseData LoadCourse(const std::string& filename) {
    CourseData course;
    
    std::ifstream file(filename);
    if (!file.is_open()){
        printfDx("�J���Ȃ�: %s\n", filename.c_str());
        return course;
    }
    json j;
    file >> j;

    course.images.startImg =
        LoadGraph(
            j["images"]["start"]
            .get<std::string>()
            .c_str()
        );

    course.images.goalImg =
        LoadGraph(
            j["images"]["goal"]
            .get<std::string>()
            .c_str()
        );

    course.images.groundImg =
        LoadGraph(
            j["images"]["ground"]
            .get<std::string>()
            .c_str()
        );

    course.images.wallImg =
        LoadGraph(
            j["images"]["wall"]
            .get<std::string>()
            .c_str()
        );

    course.images.waterImg =
        LoadGraph(
            j["images"]["water"]
            .get<std::string>()
            .c_str()
        );

    course.images.backgroundImg =
        LoadGraph(
            j["images"]["background"]
            .get<std::string>()
            .c_str()
        );

    course.start.x = j["start"]["x"];
    course.start.y = j["start"]["y"];
    course.start.width = j["start"]["width"];
    course.start.height = j["start"]["height"];

    course.goal.x = j["goal"]["x"];
    course.goal.y = j["goal"]["y"];
    //course.goal.radius = j["goal"]["radius"];
    course.goal.width = j["goal"]["width"];
    course.goal.height = j["goal"]["height"];

    course.ground.y = j["ground"]["y"];
    course.ground.height = j["ground"]["height"];


    for (const auto& obj : j["obstacles"]){
        ObstacleData obstacle;

        obstacle.type = obj["type"];
        obstacle.x = obj["x"];
        obstacle.y = obj["y"];
        obstacle.width = obj["width"];
        obstacle.height = obj["height"];
        obstacle.angle = 0;

        course.obstacles.push_back(obstacle);
    }
    return course;
}
class Input
{
public:
    void Update(){
        memcpy(OldKey, Key, sizeof(Key));
        GetHitKeyStateAll(Key);
    }

    bool Press(int key){
        return Key[key] && !OldKey[key];
    }

    bool Down(int key){//�����Ă���Ԃ����Ə�����������
        return Key[key];
    }

    bool Release(int key){//�������u�Ԃ��擾��������
        return !Key[key] && OldKey[key];
    }

private:
    char Key[256]{};
    char OldKey[256]{};
};
Input input;

void LoadFontJson(const std::string& filename){
    std::ifstream file(filename);
    //printfDx(
    //    "sheet = %s\n",
    //    sheetPath.c_str()
    //);

    if (!file.is_open()){
        return;
    }

    json j;
    file >> j;

    for (auto& item : j.items()){
        std::string ch = item.key();
        auto& data = item.value();

        auto& idle = data["idle"];
        auto& bounce = data["bounce"];

    //    std::string sheetPath =
    //idle["sheet"].get<std::string>();

//printfDx(
//    "sheet = %s\n",
//    sheetPath.c_str()
//);
//
//if (loadedSheets.find(sheetPath) == loadedSheets.end())
//{
//    loadedSheets[sheetPath] =
//        LoadGraph(sheetPath.c_str());
//
//    printfDx(
//        "handle = %d\n",
//        loadedSheets[sheetPath]
//    );
//}
        std::string sheetPath =
            idle["sheet"].get<std::string>();

        if (loadedSheets.find(sheetPath) == loadedSheets.end()){
            loadedSheets[sheetPath] =
                LoadGraph(sheetPath.c_str());
        }

        FontChar fc;

        fc.handle = loadedSheets[sheetPath];

        fc.idle.x = idle["x"];
        fc.idle.y = idle["y"];
        fc.idle.w = idle["width"];
        fc.idle.h = idle["height"];

        fc.bounce.x = bounce["x"];
        fc.bounce.y = bounce["y"];
        fc.bounce.w = bounce["width"];
        fc.bounce.h = bounce["height"];

        fontData[ch] = fc;
    }
}
void DrawBitmapString(int drawX, int drawY, const std::string& text) {
    int x = drawX;

    bool useBounce = (GetNowCount() / 200) % 2;

    for (char c : text)
    {
        std::string ch(1, c);

        auto it = fontData.find(ch);

        if (it == fontData.end()){
            printfDx(
                "not found : %s\n",
                ch.c_str()
            );
            continue;
        }

        //printfDx(
        //    "found : %s handle=%d\n",
        //    ch.c_str(),
        //    it->second.handle
        //);

        FontChar& fc = it->second;

        FontFrame& frame =
            useBounce ?
            fc.bounce :
            fc.idle;

        DrawRectGraph(
            x,
            drawY,
            frame.x,
            frame.y,
            frame.w,
            frame.h,
            fc.handle,
            TRUE
        );

        x += frame.w;
    }

    //for (char c : text){
    //    std::string ch(1, c);

    //    auto it = fontData.find(ch);

    //    if (it != fontData.end()){
    //        FontChar& fc = it->second;

    //        FontFrame& frame =
    //            useBounce ?
    //            fc.bounce :
    //            fc.idle;

    //        DrawRectGraph(
    //            x,
    //            drawY,
    //            frame.x,
    //            frame.y,
    //            frame.w,
    //            frame.h,
    //            fc.handle,
    //            TRUE
    //        );

    //        x += frame.w;
    //    }
    //}
}

void SetAnimation(Animation* anim){
    if (currentAnim != anim){//アニメの種類が変わったときに初期化
        currentAnim = anim;
        currentFrame = 0;
        frameTimer = 0;
    }
}
void SetAnimationBG(Animation* anim2) {
    if (BGAnim != anim2) {//アニメの種類が変わったときに初期化
        BGAnim = anim2;
        currentFrame2 = 0;
        frameTimer2 = 0;
    }
}
void SetAnimationITEM(Animation* anim3) {
    if (currentAnim3 != anim3) {//アニメの種類が変わったときに初期化
        currentAnim3 = anim3;
        currentFrame3 = 0;
        frameTimer3 = 0;
    }
}
void SetAnimation4(Animation* anim) {
    if (currentAnim4 != anim) {//アニメの種類が変わったときに初期化
        currentAnim4 = anim;
        currentFrame4 = 0;
        frameTimer4 = 0;
    }
}
void DrawCharacter(){
    float ballx = ball.x - 1770;//ボールが画面外にはみ出した分を計算
    float bally = -ball.y;
    //ballxの設定
    if (ball.x <= 150) {//左にはみ出る
        ballx = ball.x - 150;
    }
    else if (ball.x >= 1770) {//右にはみ出る
        ballx = ball.x - 1770;
    }
    else {
        ballx = 0;
    }
    //ballyの設定
    if (bally <= 0) {
        bally = 0;
    }

    if (!currentAnim->frames.empty()){
        Frame& f = currentAnim->frames[currentFrame];

        float scale = 0.7f;

        
        if (back) {
            DrawRectExtendGraph(
                currentAnim->drawX - (int)(f.fw * scale / 2) - (int)(f.fx * scale) + (int)(f.w * scale) - ballx,
                currentAnim->drawY - (int)(f.fh * scale / 2) - (int)(f.fy * scale),
                
                currentAnim->drawX - (int)(f.fw * scale / 2) - (int)(f.fx * scale) - ballx,
                currentAnim->drawY - (int)(f.fh * scale / 2) - (int)(f.fy * scale) + (int)(f.h * scale),
                f.x,
                f.y,
                f.w,
                f.h,

                currentAnim->imageHandle,
                TRUE
            );
        }
        else {
            DrawRectExtendGraph(
                currentAnim->drawX - (int)(f.fw * scale / 2) - (int)(f.fx * scale) - ballx,
                currentAnim->drawY - (int)(f.fh * scale / 2) - (int)(f.fy * scale),

                currentAnim->drawX - (int)(f.fw * scale / 2) - (int)(f.fx * scale) + (int)(f.w * scale) - ballx,
                currentAnim->drawY - (int)(f.fh * scale / 2) - (int)(f.fy * scale) + (int)(f.h * scale),

                f.x,
                f.y,
                f.w,
                f.h,

                currentAnim->imageHandle,
                TRUE
            );
        }
    }
}
void DrawAnimetion2() {
    if (!BGAnim->frames.empty()) {
        Frame& f = BGAnim->frames[currentFrame2];

        float scale = 1.0f;

        DrawRectExtendGraph(
            BGAnim->drawX - (int)(f.fw * scale / 2) - (int)(f.fx * scale),
            BGAnim->drawY - (int)(f.fh * scale / 2) - (int)(f.fy * scale),

            BGAnim->drawX - (int)(f.fw * scale / 2) - (int)(f.fx * scale) + (int)(f.w * scale),
            BGAnim->drawY - (int)(f.fh * scale / 2) - (int)(f.fy * scale) + (int)(f.h * scale),

            f.x,
            f.y,
            f.w,
            f.h,

            BGAnim->imageHandle,
            TRUE
        );
    }
}
void DrawAnimetion3() {
    float scale = 1.0f;
    float X = currentAnim3->drawX;
    if (playState == AIMING) {//投げた後以外はカメラOFF
    }
    if (lakecount) {
        X = ball.x;
        if (ball.x > 1770) {
            X = 1770 - 40;
        }
        else if (ball.x < 150) {
            X = 150 - 40;
        }
        else {
            X = ball.x - 40;
        }
    }
    //printfDx("ball.x = %f\n",ball.x);

    if (!currentAnim3->frames.empty()) {
        Frame& f = currentAnim3->frames[currentFrame3];
        if (lakecount) {
            DrawRectExtendGraph(
                X,
                currentAnim3->drawY - (int)(f.fh * scale / 2) - (int)(f.fy * scale),

                X + 90,
                currentAnim3->drawY - (int)(f.fh * scale / 2) - (int)(f.fy * scale) + (int)(f.h * scale),

                f.x,
                f.y,
                f.w,
                f.h,

                currentAnim3->imageHandle,
                TRUE
            );
        }
        else {
            DrawRectExtendGraph(
                X - (int)(f.fw * scale / 2) - (int)(f.fx * scale),
                currentAnim3->drawY - (int)(f.fh * scale / 2) - (int)(f.fy * scale),

                X - (int)(f.fw * scale / 2) - (int)(f.fx * scale) + (int)(f.w * scale),
                currentAnim3->drawY - (int)(f.fh * scale / 2) - (int)(f.fy * scale) + (int)(f.h * scale),

                f.x,
                f.y,
                f.w,
                f.h,

                currentAnim3->imageHandle,
                TRUE
            );

        }
    }
}
void DrawAnimetion4() {
    if (!currentAnim4->frames.empty()) {
        Frame& f = currentAnim4->frames[currentFrame4];

        float scale = 1.0f;

        DrawRectExtendGraph(
            currentAnim4->drawX - (int)(f.fw * scale / 2) - (int)(f.fx * scale),
            currentAnim4->drawY - (int)(f.fh * scale / 2) - (int)(f.fy * scale),

            currentAnim4->drawX - (int)(f.fw * scale / 2) - (int)(f.fx * scale) + (int)(f.w * scale),
            currentAnim4->drawY - (int)(f.fh * scale / 2) - (int)(f.fy * scale) + (int)(f.h * scale),

            f.x,
            f.y,
            f.w,
            f.h,

            currentAnim4->imageHandle,
            TRUE
        );
    }
}
void DrawCourseSelect() {
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
    DrawBox(0, 0, 1920, 1080, GetColor(0, 0, 0), TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    if (input.Press(KEY_INPUT_DOWN)){
        course = (course + 1) % 3;
    }

    if (input.Press(KEY_INPUT_UP)){
        course = (course + 2) % 3;
    }
    if (input.Press(KEY_INPUT_SPACE)){
        switch (course){
        case 0:
            currentCourse =
                LoadCourse("course/easy.json");
            ball.x = 500;//コース選んだ直後の最初だけ
            ball.y = currentCourse.start.y;//キャラの肩の高さ
            ball.vx = 0;
            ball.vy = 0;

            player.x = 0;

            courseLength = currentCourse.goal.x - currentCourse.start.x;
            centerX = (currentCourse.start.x + currentCourse.goal.x) / 2.0f;
            targetScale = 1920.0f / courseLength;
            coursenumber = 0;
            BGAnim = &gameBGAnim0;
            break;

        case 1:
            currentCourse =
                LoadCourse("course/normal.json");
            ball.x = 500;//コース選んだ直後の最初だけ
            ball.y = currentCourse.start.y;//偶然キャラの肩の高さ
            ball.vx = 0;
            ball.vy = 0;

            player.x = 0;

            courseLength = currentCourse.goal.x - currentCourse.start.x;
            centerX = (currentCourse.start.x + currentCourse.goal.x) / 2.0f;
            targetScale = 1920.0f / courseLength;
            coursenumber = 1;
            BGAnim = &gameBGAnim1;
            break;

        case 2:
            currentCourse =
                LoadCourse("course/hard.json");
            ball.x = 500;//コース選んだ直後の最初だけ
            ball.y = currentCourse.start.y;//キャラの肩の高さ
            ball.vx = 0;
            ball.vy = 0;

            player.x = 0;

            courseLength = currentCourse.goal.x - currentCourse.start.x;
            centerX = (currentCourse.start.x + currentCourse.goal.x) / 2.0f;
            targetScale = 1920.0f / courseLength;
            coursenumber = 2;
            BGAnim = &gameBGAnim0;
            break;
        }
        //showInputMenu = false;
        angleIndex = 1; // 0度からスタート
        previewCameraX = 0;//currentCourse.start.x - 500 ほぼ0
        previewTimer = 0;
        //courseLength = courseLength + currentCourse.start.x + 400;//ゴールの画像の幅360
        courseLength = currentCourse.goal.x + currentCourse.goal.width + 20.0f;
        targetScale = 1920.0f / courseLength;
        //printfDx(
        //    "courseLength=%.1f targetScale=%.3f\n",
        //    courseLength,
        //    targetScale
        //);
        space_cnt = 0;
        xbefor = 500;//500
        throw_cnt = 0;
        i = 1;//これ重要
        playState = EXPLAIN;
    }
    //描画処理
    DrawGraph(
        870, 250,
        course == 0 ? easySelectImg : easyImg,
        TRUE
    );
    DrawGraph(
        870, 450,
        course == 1 ? normalSelectImg : normalImg,
        TRUE
    );
    DrawGraph(
        870, 650,
        course == 2 ? hardSelectImg : hardImg,
        TRUE
    );
}
void Updateexplain() {
    //printfDx("cnt = %d \n",space_cnt);
    if (input.Press(KEY_INPUT_SPACE)){
        space_cnt++;
    }
    if (space_cnt == 4) {
        playState = COURSE_PREVIEW;
        space_cnt = 0;
    }
}
void UpdateCoursePreview() {
    if (courseLength <= 1800) {//保険
        playState = AIMING;
        return;
    }
    if (waitTimer < 120)waitTimer++;//待つ
    else {//待ち終わったら処理
        float targetX = currentCourse.goal.x - (1920.0f - currentCourse.goal.width - 20.0f);//ゴール座標は1920の倍数 - 650　だから

        previewCameraX += (targetX - previewCameraX) * 0.05f;
        if (fabs(targetX - previewCameraX) < 1.0f) {
            previewCameraX = targetX;

            previewTimer++;

            if (previewTimer > 120) {//中心に来たら待つ
                previewCameraX = targetX;
                previewTimer = 0;
                previewScale = 1.0f;
                playState = COURSE_PREVIEW_ZOOMOUT;
                waitTimer = 0;
            }
        }
    }
}
void UpdateCoursePreviewOverview(){
    //previewCameraX +=(centerX - previewCameraX) * 0.05f;//()内はマイナス。目的地までの距離を掛けてる。近いと近いほど遅い移動になる。
    previewScale +=(targetScale - previewScale) * 0.05f;//さらにフレームループで中間地点を通り過ぎることがない。
    //printfDx("scale: %d\n", courseLength);

    // 十分近付いたら
    if (fabs(targetScale - previewScale) < 0.01f){//fabs(centerX - previewCameraX) < 5.0f && 
        previewTimer++;

        if (previewTimer > 90){
            //targetScale = 1;
            playState = COURSE_PREVIEW_ZOOMIN;
            previewCameraX = (1520 + 360 - 360 * previewScale) - (currentCourse.goal.x - currentCourse.start.x) * previewScale;//終わった時のスタート位置   previewScale = targetScale 1520,360 謎
        }
    }
}
void UpdateCoursePreviewZoomIn(){
    float targetX = currentCourse.start.x;//基本500
    previewScale += (1.0f - previewScale) * 0.05f;// targetScaleに1を置いてる
    previewCameraX += (targetX - previewCameraX) * 0.05f;

    if (fabs(1.0f - previewScale) < 0.01f){
        previewTimer++;

        if (previewTimer > 120) {//待つ
            previewCameraX = 0;
            previewTimer = 0;
            previewScale = 1.0f;
            playState = AIMING;
            waitTimer = 0;
        }
    }
}
void UpdateAiming(){
    //見渡す
    //previewCameraX = player.x - 150;
    if (previewCameraX < 0) {
        previewCameraX = 0;
    }
    if (input.Down(KEY_INPUT_RIGHT)){//右
        previewCameraX += 20.0f;
        if (previewCameraX > currentCourse.goal.x - (1920.0f - currentCourse.goal.width - 20.0f)){
            previewCameraX = currentCourse.goal.x - (1920.0f - currentCourse.goal.width - 20.0f);
        }
    }
    if (input.Down(KEY_INPUT_LEFT)){//左
        previewCameraX -= 20.0f;
        if (previewCameraX < currentCourse.start.x) {//previewCameraX < player.x < player.x - 150  より　previewCameraX < player.x
            previewCameraX = currentCourse.start.x;
        }
    }
    //矢印
    if (input.Press(KEY_INPUT_UP)) {
        if (angleIndex > 0) {
            angleIndex--;
        }
    }
    if (input.Press(KEY_INPUT_DOWN)){
        if (angleIndex < 2){
            angleIndex++;
        }
    }
    if (input.Press(KEY_INPUT_RETURN)) {
        playState = ANIME0;
        space_cnt = 0;
    }
    if (input.Press(KEY_INPUT_BACK)) {
        if (back) {
            back = false;
        }
        else {
            back = true;
        }
       
    }
    if (input.Press(KEY_INPUT_SPACE)){
        previewCameraX = player.x - 500;
        if (previewCameraX < 0){
            previewCameraX = 0;
        }
        playState = WAIT_THROW;
    }
}
void UpdateWait(){
    if (input.Press(KEY_INPUT_SPACE)) {
        //previewCameraX = player.x - 150;//////////////////////
        if (previewCameraX < 0) {
            previewCameraX = 0;
        }
        waitTimer = 0;
        playState = THROW;
    }
}
void UpdateTHrowimg() {
    if (waitTimer < 45) {
        waitTimer++;//待つ
        SetAnimation(&throwAnim);
    }
    else {
        SetAnimation(&idleAnim);
    }
    throw_cnt++;
    if (waitTimer < 10)waitTimer++;//待つ
    else {
        //printfDx("x : %f  \ny : %f \n", ball.x, ball.y);
        //printfDx("xbefor : %f\n", xbefor);
        //printfDx("%d \n", i);
        //printfDx("vx : %f \nvy : %f \n", ball.vx, ball.vy);
        double dx, dy, dt = 1 / 60;
        double angle = angles[angleIndex] * 3.14159f / 180.0f; // to radians  毎回角が変わり再計算
        if (ball.y <= 720) {//地面についていないなら
            if (i == 1) {//最初だけ
                ball.vx = ball.v0 * cos(angle);//初期化
                if (back) {
                    ball.vx = -ball.v0 * cos(angle);//初期化
                }
                ball.vy = ball.v0 * sin(angle);
            }
            i += 1;
            
            // simulate until projectile hits ground (y <= 0) after leaving origin
            //while (ball.y <= 1000) {//ここは０ではなく画面一番下の座標
            dx = ball.vx * 0.1f;
            dy = ball.vy * 0.1f;//dy=-yspeed * dt;
            ball.x += dx;//ball.xは1920全然超える
            xbefor += dx;
            ball.y += dy;
            //画面右端
            if (xbefor > currentCourse.goal.x + currentCourse.goal.width + 100) { // x座標が5500以上だったらvxを逆向きにする(currentCourse.goal.x + currentCourse.goal.width + 20 )
                ball.vx = -ball.vx;
            }//画面左端
            if (xbefor < 500) { // x座標が0以下だったらvxを逆向きにする(currentCourse.goal.x + currentCourse.goal.width + 20 )
                ball.vx = -ball.vx;
            }
            for (const auto& obs : currentCourse.obstacles){
                if (obs.type != "wall")continue;//障害物が壁でないならスキップ
                    
                // 左側面
                if (xbefor < obs.x + obs.width && obs.x < xbefor && ball.vx > 0){//ボールが右から左へ動いてる
                    if (ball.y > obs.y - 40){//-40はいらないかも
                        //printfDx("hidari x : %f  y : %f \n", ball.x, ball.y);
                        //printfDx("hidari Xbefor : %f   \n", xbefor);
                        ball.vx = -ball.vx;

                        dx = ball.vx * 0.1f;
                        dy = ball.vy * 0.1f;

                        ball.x += dx;
                        xbefor += dx;
                        ball.y += dy;
                        
                    }
                }
                // 上面
                else if ( xbefor > obs.x && xbefor < obs.x + obs.width && ball.y > obs.y - 39 ){//-39はいらないかも//&& boundcount 
                    //printfDx("ue    x : %f  y : %f \n", ball.x, ball.y);
                    ball.vy = -ball.vy;
                    dx = ball.vx * 0.1f;
                    dy = ball.vy * 0.1f;
                    ball.x += dx;
                    xbefor += dx;
                    ball.y += dy;
                    boundcount = false;
                   
                }
                // 右側面
                else if (xbefor < obs.x + obs.width && obs.x < xbefor  && ball.vx < 0){//ボールが左から右へ動いてる
                    if (ball.y > obs.y - 39){//-39はいらないかも
                        //printfDx("migi   x : %f  y : %f \n", ball.x, ball.y);
                        ball.vx = -ball.vx;
                        dx = ball.vx * 0.1f;
                        dy = ball.vy * 0.1f;
                        ball.x += dx;
                        xbefor += dx;
                        ball.y += dy;
                        
                    }
                }
            }
            //cout << x << " " << y << "\n";
            /*
            if (xbefor < 2000 + 247 && 2000 < xbefor && ball.vx > 0) { //障害物左衝突判定
                if (ball.y > 440) {
                    ball.vx = -ball.vx;
                    dx = ball.vx * 0.1f;
                    dy = ball.vy * 0.1f;//dy=-yspeed * dt;
                    ball.x += dx;//ball.xは1920全然超える
                    xbefor += dx;
                    ball.y += dy;
                }
            }
            if (xbefor > 2000 && xbefor < 2000 + 247 && ball.y > 441 && boundcount) { // 障害物の上に来た判定
                // printfDx("kiteruyokiteruyokiteruyoooooooooooooooooooooooooooooooooooooooooooooooooooooooooo");
                ball.vy = -ball.vy;
                dx = ball.vx * 0.1f;
                dy = ball.vy * 0.1f;//dy=-yspeed * dt;
                ball.x += dx;//ball.xは1920全然超える
                xbefor += dx;
                ball.y += dy;
                boundcount = false;//    ここ消したら、壁の上滑る

            }
            if (xbefor < 2000 + 247 && 2000 < xbefor && ball.vx < 0) { //障害物右衝突判定、左衝突と違うのはball.vyの判定
                if (ball.y > 441) {
                    ball.vx = -ball.vx;
                    dx = ball.vx * 0.1f;
                    dy = ball.vy * 0.1f;//dy=-yspeed * dt;
                    ball.x += dx;//ball.xは1920全然超える
                    xbefor += dx;
                    ball.y += dy;
                }
            }*/
            ball.vy += 9.8f * 0.1f;
        }
        else if (ball.vx != 0) {//地面についてるとき　　止まってない
            for (const auto& obs : currentCourse.obstacles){
                if (obs.type != "water") continue;

                if (xbefor > obs.x && xbefor < obs.x + obs.width){
                    lakecount = true;

                    ball.vx = 0;
                    ball.vx = ball.v0 * cos(angle);//初期化
                    if (back) {
                        ball.vx = -ball.v0 * cos(angle);//初期化
                    }
                    ball.vy = ball.v0 * sin(angle);

                    waitTimer = 0;
                    i = 1;
                    currentFrame3 = 0;
                    frameTimer3 = 0;
                    playState = THROWEND;

                    break;
                }
            }
            //if (xbefor < 2610 + 360 && 2610 < xbefor) { //池の前方2820px,幅820px
            //    lakecount = true;
            //    //printfDx("11111111111111111111111111111111111111111111111111111111111111");
            //    ball.vx = 0;
            //    ball.vx = ball.v0 * cos(angle);//初期化
            //    ball.vy = ball.v0 * sin(angle);
            //    //cameraTargetX = ball.x - 960;
            //    waitTimer = 0;
            //    i = 1;
            //    currentFrame3 = 0;
            //    frameTimer3 = 0;
            //    playState = THROWEND;
            //}
            if (currentCourse.goal.x + currentCourse.goal.width > xbefor && xbefor > currentCourse.goal.x) {//ゴールのとき
                goalcount = true;
                ball.vx = 0;
                ball.vx = ball.v0 * cos(angle);//初期化
                if (back) {
                    ball.vx = -ball.v0 * cos(angle);//初期化
                }
                ball.vy = ball.v0 * sin(angle);
                //printfDx("gggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggg");
                playState = RESULT;
            }
            else {//池じゃないとき
                ball.vy = -ball.vy * 0.6f;
                if (ball.vy > -10) {
                    ball.vy = 0;
                    ball.y = 721;
                    if (ball.vx != 0) {
                        ball.vx *= 0.9f;
                        if (abs(ball.vx) < 1) {//投球終わり
                            ball.vx = 0;
                            ball.vx = ball.v0 * cos(angle);//初期化
                            if (back) {
                                ball.vx = -ball.v0 * cos(angle);//初期化
                            }
                            
                            ball.vy = ball.v0 * sin(angle);
                            //cameraTargetX = ball.x - 960;
                            waitTimer = 0;
                            i = 1;
                            currentFrame3 = 0;
                            frameTimer3 = 0;
                            playState = THROWEND;
                        }
                    }
                }
            }
            dx = ball.vx * 0.1f;
            dy = ball.vy * 0.1f;//dy=-yspeed * dt;
            ball.x += dx;//ball.xは1920全然超える
            xbefor += dx;
            ball.y += dy;
        }
    }
}
/*   
void UpdateTHrowimg() {
    if (waitTimer < 45) {
        waitTimer++;//待つ
        SetAnimation(&throwAnim);
    }
    else {
        SetAnimation(&idleAnim);
    }
    throw_cnt++;
    if (waitTimer < 10)waitTimer++;//待つ
    else {
        printfDx("x : %f  \ny : %f \n", ball.x, ball.y);
        printfDx("i :%d \n", i);
        printfDx("xbefor : %f\n", xbefor);
        double dx, dy, dt = 1 / 60;
        double angle = angles[angleIndex] * 3.14159f / 180.0f; // to radians  毎回角が変わり再計算
        if (ball.y <= 720) {//地面についていないなら
            if (i == 1) {//最初だけ
                ball.vx = ball.v0 * cos(angle);
                ball.vy = ball.v0 * sin(angle);
            }
            i += 1;
            printfDx("%d \n", i);

            //printfDx("vx : %f \nvy : %f \n", ball.vx, ball.vy);
            // simulate until projectile hits ground (y <= 0) after leaving origin
            //while (ball.y <= 1000) {//ここは０ではなく画面一番下の座標
            dx = ball.vx * 0.1f;
            dy = ball.vy * 0.1f;//dy=-yspeed * dt;
            ball.x += dx;//ball.xは1920全然超える
            xbefor += dx;
            ball.y += dy;

            if (xbefor > 5380 + 360 + 500) { // x座標が5500以上だったらvxを逆向きにする(currentCourse.goal.x + currentCourse.goal.width + 20 )
                ball.vx = -ball.vx;
            }
            if (xbefor < 500) { // x座標が0以下だったらvxを逆向きにする(currentCourse.goal.x + currentCourse.goal.width + 20 )
                ball.vx = -ball.vx;
            }

            //printfDx("ballx : %f \nxbefor : %f \n", ball.x, xbefor);
            //cout << x << " " << y << "\n";
            if (xbefor < 2000 + 247 && 2000 < xbefor && ball.vx > 0) { //障害物左衝突判定
                if (ball.y > 440) {
                    ball.vx = -ball.vx;
                    dx = ball.vx * 0.1f;
                    dy = ball.vy * 0.1f;//dy=-yspeed * dt;
                    ball.x += dx;//ball.xは1920全然超える
                    xbefor += dx;
                    ball.y += dy;
                }
            }
            if (xbefor > 2000 && xbefor < 2000 + 247 && ball.y > 441 && boundcount) { // 障害物の上に来た判定
                // printfDx("kiteruyokiteruyokiteruyoooooooooooooooooooooooooooooooooooooooooooooooooooooooooo");
                ball.vy = -ball.vy;
                dx = ball.vx * 0.1f;
                dy = ball.vy * 0.1f;//dy=-yspeed * dt;
                ball.x += dx;//ball.xは1920全然超える
                xbefor += dx;
                ball.y += dy;
                boundcount = false;//    ここ消したら、壁の上滑る

            }
            if (xbefor < 2000 + 247 && 2000 < xbefor && ball.vx < 0) { //障害物右衝突判定、左衝突と違うのはball.vyの判定
                if (ball.y > 441) {
                    ball.vx = -ball.vx;
                    dx = ball.vx * 0.1f;
                    dy = ball.vy * 0.1f;//dy=-yspeed * dt;
                    ball.x += dx;//ball.xは1920全然超える
                    xbefor += dx;
                    ball.y += dy;
                }
            }
            ball.vy += 9.8f * 0.1f;
        }
        else if (ball.vx != 0) {//地面についてるとき　　止まってない
            if (xbefor < 2610 + 360 && 2610 < xbefor) { //池の前方2820px,幅820px
                lakecount = true;
                //printfDx("11111111111111111111111111111111111111111111111111111111111111");
                ball.vx == 0;
                ball.vx = ball.v0 * cos(angle);//初期化
                ball.vy = ball.v0 * sin(angle);
                //cameraTargetX = ball.x - 960;
                waitTimer = 0;
                i = 1;
                currentFrame3 = 0;
                frameTimer3 = 0;
                playState = THROWEND;
            }

            else {
                ball.vy = -ball.vy * 0.6f;
                if (ball.vy > -10) {
                    ball.vy = 0;
                    ball.y = 721;
                    if (ball.vx != 0) {
                        ball.vx *= 0.9f;
                        if (abs(ball.vx) < 1) {//投球終わり
                            ball.vx == 0;
                            if (5380 + 360 + 120 > xbefor && xbefor > 5380 + 120) { //+200は調整
                                goalcount = true;
                                printfDx("gggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggg");
                                playState = RESULT;
                            }
                            ball.vx = ball.v0 * cos(angle);//初期化
                            ball.vy = ball.v0 * sin(angle);
                            //cameraTargetX = ball.x - 960;
                            waitTimer = 0;
                            i = 1;
                            currentFrame3 = 0;
                            frameTimer3 = 0;
                            playState = THROWEND;
                        }
                    }
                }

            }
            dx = ball.vx * 0.1f;
            dy = ball.vy * 0.1f;//dy=-yspeed * dt;
            ball.x += dx;//ball.xは1920全然超える
            xbefor += dx;
            ball.y += dy;
        }
    }
}*/
void UpdateThrowend() {//ball.x → 1920/20(画面中央)
    //足すなら演出
    //SplashAnim.drawX = ball.x - 50;
    //スペース押されるまで描画のみ
    if (input.Press(KEY_INPUT_SPACE)) {
        if (lakecount) {//lakecount(池に落ちたなら)がある(true)なら
            playState = AIMING;
            lakecount = false;
            player.x += 0;
            xbefor = player.x + 0;//投げる前(加算する前)のプレイヤー座標に戻す。ボールはその+500
            ball.x = 500;//画面上でのいつもの定位置
            ball.y = currentCourse.start.y;
            previewCameraX = player.x - 500;
            boundcount = true;
        }
        else {//池に落ちてないなら
            //previewCameraX = 0;//初期化
            playState = AIMING;
            
            //player.x += ball.x - 500;//画面関係なく進んだx座標  x = 500 から投げたから,ball.x = 3000　とかから引く。それが進んだ分。それをplayerのx座標に足す。
            player.x = xbefor;
            
            
            
            ball.x = 500;//コースセレクトと同じ値  画面に表示する用だから1920より必ず小さい　　初期化　投げるスタート地点
            
            ball.y = currentCourse.start.y;//上に同じ
            //if ((currentCourse.goal.x + currentCourse.goal.width + 20) - player.x <= 1920) {//(currentCourse.goal.x + currentCourse.goal.width + 20)はコース全体。から今のplayer位置を引く。これが画面1920より小さかったら
            //    previewCameraX = (currentCourse.goal.x + currentCourse.goal.width + 20) - 1920;//最後の一枚(1920)を除いてカメラで動かす
            //}
            if (xbefor < 500) {//
                player.x = 0;
                xbefor = 500;
                ball.x = 500;
            }
            for (const auto& obs : currentCourse.obstacles) {
                if (obs.type != "wall")continue;//障害物が壁でないならスキップ

                if ((xbefor < obs.x + obs.x + 200) && (obs.x < xbefor) ) {//プレイヤーが壁、池の上に立たないようにずらす。
                    xbefor = obs.x + obs.width + 200;
                }
                else if(xbefor < obs.x + obs.width + 200 && obs.x < xbefor) {
                    xbefor = obs.x + obs.width + 200;
                }
                player.x = xbefor;
                
            }
            previewCameraX = player.x - 500;//進んだx座標 - 150　を画面端に。
            if (previewCameraX < 0) {
                previewCameraX = 0;
            }
        }
    }
}
void UpdateResult(){
    if (input.Press(KEY_INPUT_SPACE)) {
            //previewCameraX = 0;//初期化
        currentState = MENU;
        playState = COURSE_SELECT;

            player.x = 0;//画面関係なく進んだx座標  x = 500 から投げたから,ball.x = 3000　とかから引く。それが進んだ分。それをplayerのx座標に足す。
            if (player.x) {//プレイヤーが壁、池の上に立たないようにずらす。

            }
            ball.x = 500;//コースセレクトと同じ値  画面に表示する用だから1920より必ず小さい　　初期化　投げるスタート地点

            ball.y = currentCourse.start.y;//上に同じ
            //if ((currentCourse.goal.x + currentCourse.goal.width + 20) - player.x <= 1920) {//(currentCourse.goal.x + currentCourse.goal.width + 20)はコース全体。から今のplayer位置を引く。これが画面1920より小さかったら
            //    previewCameraX = (currentCourse.goal.x + currentCourse.goal.width + 20) - 1920;//最後の一枚(1920)を除いてカメラで動かす
            //}
            previewCameraX = player.x;//進んだx座標 - 150　を画面端に。
        
    }
    //printfDx("RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR \n");
}
void UpAnime0() {
    //printfDx("cnt = %d \n", space_cnt);
    
    switch (space_cnt){
    case 0://なんも押してないExplaincatAnim
        SetAnimationITEM(&AnimItem1_2);
        //SetAnimationITEM(&ExplaincatAnim);
        if (input.Press(KEY_INPUT_SPACE)) {
          if (currentFrame3) {//成功
              space_cnt++;
              waitTimer = 0;
          }
        }
        break;

    case 1:
        if (waitTimer < 80) {
            waitTimer++;//待つ
            SetAnimationITEM(&AnimItem2_1);
        }
        else {
            SetAnimationITEM(&AnimItem2_2);
        }
        break;

    case 2:
        if (waitTimer < 80) {
            waitTimer++;//待つ
            SetAnimationITEM(&AnimItem3_1);
        }
        else {
            SetAnimationITEM(&AnimItem3_2);
        }
        break;

    case 3:
        if (waitTimer < 80) {
            waitTimer++;//待つ
            SetAnimationITEM(&AnimItem4_1);
        }
        else {
            SetAnimationITEM(&AnimItem4_2);
        }
        break;
    case 4:
        if (waitTimer < 80) {
            waitTimer++;//待つ
            SetAnimationITEM(&AnimItem5_1);
        }
        else {
            SetAnimationITEM(&AnimItem5_2);
        };
        break;
    case 5:
        if (waitTimer < 80) {
            waitTimer++;//待つ
            SetAnimationITEM(&AnimItem6_1);
        }
        else {
            SetAnimationITEM(&AnimItem6_2);
        }
        break;
    case 6:
        if (waitTimer < 80) {
            waitTimer++;//待つ
            SetAnimationITEM(&AnimItem2_1);
        }
        else {
            SetAnimationITEM(&AnimItem2_2);
        }
        break;

    case 7:
        playState = WAIT_THROW;
        space_cnt = 0;
        break;
    }
}
void LoadAnimationXML(const char* xmlPath, std::vector<Frame>&anim) {
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(xmlPath) != XML_SUCCESS) return;

    tinyxml2::XMLElement* atlas = doc.FirstChildElement("TextureAtlas");
    if (!atlas) return;
    tinyxml2::XMLElement* sub = atlas->FirstChildElement("SubTexture");

    while (sub) {//sub は <SubTexture> を指すポインタ
        const char* name = sub->Attribute("name");//変数nameにXMLのname属性の文字列を入れる
        Frame f;
        sub->QueryIntAttribute("x", &f.x);//xの値をFrame(f)グループのxの項目に入れる
        sub->QueryIntAttribute("y", &f.y);//&はポインタ
        sub->QueryIntAttribute("width", &f.w);
        sub->QueryIntAttribute("height", &f.h);

        sub->QueryIntAttribute("frameX", &f.fx);
        sub->QueryIntAttribute("frameY", &f.fy);
        sub->QueryIntAttribute("frameWidth", &f.fw);
        sub->QueryIntAttribute("frameHeight", &f.fh);//push_backはvector の一番うしろにデータを追加する関数

        anim.push_back(f);

        sub = sub->NextSiblingElement("SubTexture");
    }
}
void DrawProgressMap(){
    int mapX = 1400;
    int mapY = 60;
    int mapWidth = 400;
    float  progress = (ball.x + player.x - currentCourse.start.x - 500) / (float)(currentCourse.goal.x - currentCourse.start.x);

    // �S�̐�
    DrawLine(
        mapX,
        mapY,
        mapX + mapWidth,
        mapY,
        GetColor(0, 0, 0)
    );

    // 線
    DrawLine(
        mapX,
        mapY,
        mapX + (int)(mapWidth * progress),
        mapY,
        GetColor(105, 105, 105)
    );
    // スタート
    DrawCircle(
        mapX,
        mapY,
        5,
        GetColor(0, 0, 0),
        TRUE
    );
    // ゴール
    DrawGraph(
        mapX + mapWidth - 16,
        mapY - 20,
        miniFlagImg,
        TRUE
    );//��Q��
    for (const auto& obs : currentCourse.obstacles){
        float rate =
            (obs.x - currentCourse.start.x)
            / (float)(currentCourse.goal.x - currentCourse.start.x);

        int obsPos =
            mapX + (int)(mapWidth * rate);

        int image = -1;
        int imagey = 0;

        if (obs.type == "wall"){
            image = miniWallImg;
            imagey = 51 / 2;
        }
        else if (obs.type == "water"){
            image = miniWaterImg;
            imagey = 28 / 2;
        }
        if (image != -1){
            DrawGraph(
                obsPos - 8,
                mapY - imagey,
                image,
                TRUE
            );
        }
    }
    // 人
    int playerPos =
        mapX + (int)(mapWidth * progress);

    DrawGraph(
        playerPos - 16,
        mapY - 20,
        miniHumanImg,
        TRUE
    );
}
void DrawCoursePreview(){

    for (int x = -1920; x < currentCourse.goal.x + 1920; x += 1920) {
        DrawGraph(
            x - previewCameraX,
            1020 - currentCourse.ground.height,//1080 - 60
            currentCourse.images.groundImg,
            TRUE
        );
    }
    DrawGraph(// ゴール
        currentCourse.goal.x - previewCameraX,
        currentCourse.goal.y,
        currentCourse.images.goalImg,
        TRUE
    );
    DrawGraph(//スタート
        currentCourse.start.x - previewCameraX,
        currentCourse.start.y,
        currentCourse.images.startImg,
        TRUE
    );

    // 障害物
    for (const auto& obs : currentCourse.obstacles){
        int image = -1;

        if (obs.type == "wall"){
            image = currentCourse.images.wallImg;
        }
        else if (obs.type == "water"){
            image = currentCourse.images.waterImg;
        }
        if (image != -1){
            DrawGraph(
                obs.x - previewCameraX,
                obs.y,
                image,
                TRUE
            );
        }
    }
}
void DrawCoursePreviewOverview() {
    int imgNumber = currentCourse.goal.x / 1920 + 2;//地面の画像の枚数。
    float groundScale = previewScale * 1.0;
    //printfDx( "maisuu=%d\n",imgNumber);
    for (float x = 1920; x > -1920; x -= 1920 * groundScale) {
        DrawExtendGraph(
            x - 1920 * groundScale,
            1020 - currentCourse.ground.height * groundScale,
            x,
            1020,//1080 - (下の説明バーの高さ)

            currentCourse.images.groundImg,
            TRUE
        );
    }   
    // ゴール                                                                                      70は画像と画面端をあけるため
    float goalx = currentCourse.goal.x - (currentCourse.goal.x - (1920 - currentCourse.goal.width - 0)) + currentCourse.goal.width - currentCourse.goal.width * previewScale; //previewCameraX = currentCourse.goal.x - 1400(最終的に)  1400が1520に、さらに1270
    DrawExtendGraph(
        goalx,//goalx = 1400 + 360 - 360 * previewScale
        //(currentCourse.goal.x - (currentCourse.goal.x - 1400.0f)) * previewScale,
        currentCourse.goal.y + currentCourse.goal.height - currentCourse.goal.height * previewScale,//* previewScale

        currentCourse.goal.x - (currentCourse.goal.x - (1920 - currentCourse.goal.width - 0)) + currentCourse.goal.width,//* previewScale
        currentCourse.goal.y + currentCourse.goal.height,//

        currentCourse.images.goalImg,
        TRUE
    );
    DrawExtendGraph(//スタート    
        ((1920 - currentCourse.goal.width - 70)) + currentCourse.goal.width - currentCourse.goal.width * previewScale - (currentCourse.goal.x - currentCourse.start.x) * previewScale,
        currentCourse.start.y + currentCourse.start.height - currentCourse.start.height * previewScale,

        goalx - (currentCourse.goal.x - currentCourse.start.x) * previewScale + currentCourse.start.width * previewScale,
        currentCourse.start.y + currentCourse.start.height,//* previewScale　どうせ1020

        currentCourse.images.startImg,
        TRUE
    );
    // 障害物
    for (const auto& obs : currentCourse.obstacles) {
        int image = -1;

        if (obs.type == "wall") {
            image = currentCourse.images.wallImg;
        }
        else if (obs.type == "water") {
            image = currentCourse.images.waterImg;
        }
        if (image != -1) {
            DrawExtendGraph(
                goalx - (currentCourse.goal.x - obs.x) * previewScale,
                1020 - obs.height * previewScale,

                goalx - (currentCourse.goal.x - obs.x) * previewScale + obs.width * previewScale,
                1020 ,//obs.y + obs.height

                image,
                TRUE
            );
        }
    }
}
void DrawCoursePreviewZoomIn() {
    int imgNumber = currentCourse.goal.x / 1920 + 2;//地面の画像の枚数。
    for (float x = 0; x < 1920; x += 1920 * previewScale) {
        DrawExtendGraph(
            x ,
            1020 - currentCourse.ground.height * previewScale,
            x + 1920 * previewScale,
            1020,

            currentCourse.images.groundImg,
            TRUE
        );
    }
   // float goalx = currentCourse.goal.x - (currentCourse.goal.x - 1400) + 360 - 360 * previewScale;//1400 + 360 - 360 * previewScale
    float startx = previewCameraX;
    DrawExtendGraph(
        startx + (currentCourse.goal.x - currentCourse.start.x) * previewScale,
        //(currentCourse.goal.x - (currentCourse.goal.x - 1400.0f)) * previewScale, //previewCameraX = currentCourse.goal.x - 1400(最終的に)
        currentCourse.goal.y + currentCourse.goal.height - currentCourse.goal.height * previewScale,//* previewScale

        startx + (currentCourse.goal.x - currentCourse.start.x) * previewScale + currentCourse.goal.width,//* previewScale        + previewCameraX
        currentCourse.goal.y + currentCourse.goal.height,//

        currentCourse.images.goalImg,
        TRUE
    );
    
    DrawExtendGraph(//スタート    previewCameraX += (targetX - previewCameraX) * 0.05f;
        startx,
        currentCourse.start.y + currentCourse.start.height - currentCourse.start.height * previewScale,

        startx + currentCourse.start.width * previewScale,
        currentCourse.start.y + currentCourse.start.height,//* previewScale

        currentCourse.images.startImg,
        TRUE
    );
    // 障害物
    for (const auto& obs : currentCourse.obstacles) {
        int image = -1;

        if (obs.type == "wall") {
            image = currentCourse.images.wallImg;
        }
        else if (obs.type == "water") {
            image = currentCourse.images.waterImg;
        }

        if (image != -1) {
            DrawExtendGraph(
                startx + (obs.x - currentCourse.start.x) * previewScale,
                1020 - obs.height * previewScale,

                startx+ (obs.x - currentCourse.start.x) * previewScale + obs.width * previewScale,
                1020,//* previewScale

                image,
                TRUE
            );
        }
    }
}
void DrawCourse(){
    if (coursenumber == 2) {
        DrawGraph(
            0,
            -60,
            HaikeiImg,
            TRUE
        );
    }
    float ballx = ball.x - 1770;//ボールが画面外にはみ出した分を計算
    float bally = -ball.y;
    //ballxの設定
    if (ball.x <= 150) {//左にはみ出る
        ballx = ball.x - 150;
    }
    else if (ball.x >= 1770) {//右にはみ出る
        ballx = ball.x - 1770;
    }
    else {
        ballx = 0;
    }
    //ballyの設定
    if (bally <= 0) {
        bally = 0;
    }    
    if (lakecount) {
        DrawAnimetion3();
    }
    int waterX = -1;
    for (const auto& obs : currentCourse.obstacles){
        if (obs.type == "water"){
            waterX = obs.x;
            break;
        }
    }
    for (int x = 0; x < currentCourse.goal.x + 1920; x += 1920){
        int groundImg = currentCourse.images.groundImg;//画像の入れ物
        if (coursenumber != 4) {// 
            if (waterX != -1){
                if (x <= waterX && waterX < x + 1920){
                    groundImg = currentCourse.images.waterImg;
                }
            }
        }
        DrawGraph(
            x - previewCameraX - ballx,
            1020 - currentCourse.ground.height,
            groundImg,
            TRUE
        );
    }
    //printfDx("%f\n", previewCameraX);
    // ゴール
    DrawGraph(
        currentCourse.goal.x - previewCameraX - ballx,
        currentCourse.goal.y,
        currentCourse.images.goalImg,
        TRUE
    );
    DrawGraph(//スタート
        currentCourse.start.x - previewCameraX - ballx,
        currentCourse.start.y,
        currentCourse.images.startImg,
        TRUE
    );

    
    // 障害物
    for (const auto& obs : currentCourse.obstacles){
        int image = -1;

        if (obs.type == "wall"){
            image = currentCourse.images.wallImg;
        }
        else if (obs.type == "water"){
            //if (coursenumber != 2) {//hardコースでないなら
            //    image = currentCourse.images.waterImg;//hardコースにwaterはない
            //}
        }
        if (image != -1){
            DrawGraph(
                obs.x - previewCameraX - ballx,
                obs.y,
                image,
                TRUE
            );
        }
    }
    if (coursenumber == 2) {
        DrawGraph(
            0,
            -60,
            TemaeImg,
            TRUE
        );
    }
}
void DrawAiming() {
    int angleImg = angle0Img;

    switch (angleIndex){
    case 0:
        angleImg = angle45Img;
        break;

    case 1:
        angleImg = angle0Img;
        break;

    case 2:
        angleImg = angleMinus45Img;
        break;
    }
    DrawGraph(
        400,
        300,
        angleImg,
        TRUE
    );
}
void DrawBall() {
    float ballx = ball.x;
    float bally = ball.y;
    float hyaku1 = 0.0f;
    float hyaku2 = 1080.0f;
    float ballcamerax = previewCameraX;
    if (ball.x >= 1770) {//ボールが画面外出るなら
        ballx = 1770;//ボールに画面を追従する
    }
    else if(ball.x <= 150){
        ballx = 150;
    }
    if (bally <= -10) {//上に行きすぎたら というより代わりの画像の高さになったら0 - (画像の高さ)
        hyaku1 = 1080;//消す
        hyaku2 = 10;
    }
    if (playState == THROW) {//ボールが飛んでるときはカメラOFF
        ballcamerax = 0;
    }
    hyaku1 = ball.y;
    //printfDx("player.x : %f \n", player.x);
    //printfDx("ball x : %f \n", previewCameraX);
    DrawGraph(
        ballx - ballcamerax,
        hyaku1,
        BallImg,
        TRUE
    );//
    DrawGraph(
        ballx - ballcamerax,
        hyaku2,
        miniHumanImg,
        TRUE
    );
}
void Throwend() {
    float ballx = ball.x;
    //float bally = ball.y;
    float hyaku1 = 0.0f;
    float hyaku2 = 1080.0f;
    float ballcamerax = previewCameraX;
    if (ball.x >= 1770) {//ボールが画面外出るなら
        ballx = 1770;//ボールに画面を追従する
    }
    else if (ball.x <= 150) {
        ballx = 150;
    }
    //if (bally <= -10) {//上に行きすぎたら というより代わりの画像の高さになったら0 - (画像の高さ)
    //    hyaku1 = 1080;//消す
    //    hyaku2 = 10;
    //}
    if (playState == THROW) {//ボールが飛んでるときはカメラOFF
        ballcamerax = 0;
    }
    hyaku1 = ball.y;
    DrawGraph(
        ballx,// ballx - ballcamerax
        ball.y,
        miniFlagImg,
        TRUE
    );
}
void DrawExplain() {
    int ExImg = Explain1Img;
    switch (space_cnt) {
    case 0:
        ExImg = Explain1Img;
        break;
    case 1:
        ExImg = Explain2Img;
        break;
    case 2:
        ExImg = Explain3Img;
        break;
    case 3:
        ExImg = Explain4Img;
        break;
    }
    if (waitTimer < 2)waitTimer++;//待つ
    else {//待ち終わったら処理
        //SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
        //DrawBox(0, 0, 1920, 1080, GetColor(0, 0, 0), TRUE);
        //SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

        //DrawAnimetion4();//ねこあにめ

        DrawGraph(
            0,
            0,
            ExImg,
            TRUE
        );
    }
}
void DrawResult() {
    if (waitTimer < 120)waitTimer++;//描画を待つ
    else {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
        DrawBox(0, 0, 1920, 1080, GetColor(0, 0, 0), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        //文字
        DrawAnimetion4();//これも120待ってからねこ

    }
}
void DrawResulting() {
    if (waitTimer < 120)waitTimer++;//描画を待つ
    else {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
        DrawBox(0, 0, 1920, 1080, GetColor(0, 0, 0), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        //文字
        DrawAnimetion4();//これも120待ってから
    }
}
void DrawRect() {
    //printfDx("Timer = % f\n",waitTimer);
    DrawAnimetion3();
}
void DrawData() {
    int kyori = xbefor;
    std::string str = std::to_string(kyori);
    DrawBitmapString(
        50,
        18,
        "NOW:"
    );
    DrawBitmapString(
        370,//50 + 80*4
        18,
        str.c_str()
    );
}
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    ChangeWindowMode(TRUE);
    SetGraphMode(1920, 1080, 32);

    if (DxLib_Init() == -1) {
        return -1;
    }
    int menuBG = LoadGraph("image/background0.png");
    LoadFontJson("assets/font.json");
    //printfDx("font size=%d\n", (int)fontData.size());
    gameBG = LoadGraph("image/background1.png");
    easyImg = LoadGraph("image/course_easy.png");
    normalImg = LoadGraph("image/course_normal.png");
    hardImg = LoadGraph("image/course_hard.png");
    hederImg = LoadGraph("image/heder.png");
    TemaeImg = LoadGraph("image//hard/temae.png");
    HaikeiImg = LoadGraph("image/hard/huukei.png");
    BallImg = LoadGraph("image/hard/ball.png");

    Explain1Img = LoadGraph("image/hard/explain1.png");
    Explain2Img = LoadGraph("image/hard/explain2.png");
    Explain3Img = LoadGraph("image/hard/explain3.png");
    Explain4Img = LoadGraph("image/hard/explain4.png");

    easySelectImg = LoadGraph("image/course_easy_select.png");
    normalSelectImg = LoadGraph("image/course_normal_select.png");
    hardSelectImg = LoadGraph("image/course_hard_select.png");

    miniHumanImg = LoadGraph("image/icon_hito.png");
    miniFlagImg = LoadGraph("image/icon_goal.png");
    miniWallImg = LoadGraph("image/icon_wall.png");
    miniWaterImg = LoadGraph("image/icon_water.png");

    //WallImg = LoadGraph("image/wall.png");
    //WaterImg = LoadGraph("image/water.png");
    //GoalImg = LoadGraph("image/goal.png");
    //StartImg = LoadGraph("image/start.png");
    //GroundImg = LoadGraph("image/grass.png");
    GroundCoverImg = LoadGraph("image/grassCover.png");//不要

    angle0Img = LoadGraph("image/angle0Img.png");
    angle45Img = LoadGraph("image/angle45Img.png");
    angleMinus45Img = LoadGraph("image/angleMinus45Img.png");

    idleAnim.imageHandle = LoadGraph("image/anim/idle.png");
    readyAnim.imageHandle = LoadGraph("image/anim/ready.png");
    throwAnim.imageHandle = LoadGraph("image/anim/throw.png");
    gameBGAnim0.imageHandle = LoadGraph("image/anim/gameBG.png");
    gameBGAnim1.imageHandle = LoadGraph("image/anim/gameBG1.png");
    //AnimItem0.imageHandle = LoadGraph("image/anim/rect0.png");
    AnimItem1_1.imageHandle = LoadGraph("image/anim/rect1-1.png");
    AnimItem2_1.imageHandle = LoadGraph("image/anim/rect2-1.png");
    AnimItem3_1.imageHandle = LoadGraph("image/anim/rect3-1.png");
    AnimItem4_1.imageHandle = LoadGraph("image/anim/rect4-1.png");
    AnimItem5_1.imageHandle = LoadGraph("image/anim/rect5-1.png");
    AnimItem6_1.imageHandle = LoadGraph("image/anim/rect6-1.png");

    AnimItem1_2.imageHandle = LoadGraph("image/anim/rect1-2.png");
    AnimItem2_2.imageHandle = LoadGraph("image/anim/rect2-2.png");
    AnimItem3_2.imageHandle = LoadGraph("image/anim/rect3-2.png");
    AnimItem4_2.imageHandle = LoadGraph("image/anim/rect4-2.png");
    AnimItem5_2.imageHandle = LoadGraph("image/anim/rect5-2.png");
    AnimItem6_2.imageHandle = LoadGraph("image/anim/rect6-2.png");

    ExplaincatAnim.imageHandle = LoadGraph("image/anim/explaincat.png");
    CheercatAnim.imageHandle = LoadGraph("image/anim/cheercat.png");
    SplashAnim.imageHandle = LoadGraph("image/anim/splash.png");
    TryAnim.imageHandle = LoadGraph("image/anim/try.png");
    WatercatAnim.imageHandle = LoadGraph("image/anim/watercat.png");
    fontSheetHandle = LoadGraph("assets/images/font1.png");

    bgmHandle = LoadSoundMem("sound/littleidea.ogg");

    LoadAnimationXML("image/anim/idle.xml", idleAnim.frames);
    LoadAnimationXML("image/anim/ready.xml", readyAnim.frames);
    LoadAnimationXML("image/anim/throw.xml", throwAnim.frames);
    LoadAnimationXML("image/anim/gameBG.xml", gameBGAnim0.frames);
    LoadAnimationXML("image/anim/gameBG1.xml", gameBGAnim1.frames);
    //LoadAnimationXML("image/anim/rect0.xml", AnimItem0.frames);
    LoadAnimationXML("image/anim/rect1-1.xml", AnimItem1_1.frames);
    LoadAnimationXML("image/anim/rect2-1.xml", AnimItem2_1.frames);
    LoadAnimationXML("image/anim/rect3-1.xml", AnimItem3_1.frames);
    LoadAnimationXML("image/anim/rect4-1.xml", AnimItem4_1.frames);
    LoadAnimationXML("image/anim/rect5-1.xml", AnimItem5_1.frames);
    LoadAnimationXML("image/anim/rect6-1.xml", AnimItem6_1.frames);
    LoadAnimationXML("image/anim/rect1-2.xml", AnimItem1_2.frames);
    LoadAnimationXML("image/anim/rect2-2.xml", AnimItem2_2.frames);
    LoadAnimationXML("image/anim/rect3-2.xml", AnimItem3_2.frames);
    LoadAnimationXML("image/anim/rect4-2.xml", AnimItem4_2.frames);
    LoadAnimationXML("image/anim/rect5-2.xml", AnimItem5_2.frames);
    LoadAnimationXML("image/anim/rect6-2.xml", AnimItem6_2.frames);

    LoadAnimationXML("image/anim/explaincat.xml", ExplaincatAnim.frames);
    LoadAnimationXML("image/anim/cheercat.xml", CheercatAnim.frames);
    LoadAnimationXML("image/anim/splash.xml", SplashAnim.frames);
    LoadAnimationXML("image/anim/try.xml", TryAnim.frames);
    LoadAnimationXML("image/anim/watercat.xml", WatercatAnim.frames);

    idleAnim.drawX = 400;
    idleAnim.drawY = 650;

    readyAnim.drawX = 370;
    readyAnim.drawY = 650;

    throwAnim.drawX = 400;
    throwAnim.drawY = 550;

    gameBGAnim0.drawX = 960;
    gameBGAnim0.drawY = 540;

    gameBGAnim1.drawX = 960;
    gameBGAnim1.drawY = 540;

    //AnimItem0.drawX = 960;
    //AnimItem0.drawY = 540;
    AnimItem1_1.drawX = 960;
    AnimItem1_1.drawY = 540;
    AnimItem2_1.drawX = 960;
    AnimItem2_1.drawY = 540;
    AnimItem3_1.drawX = 960;
    AnimItem3_1.drawY = 540;
    AnimItem4_1.drawX = 960;
    AnimItem4_1.drawY = 540;
    AnimItem5_1.drawX = 960;
    AnimItem5_1.drawY = 540;
    AnimItem6_1.drawX = 960;
    AnimItem6_1.drawY = 540;

    AnimItem1_2.drawX = 960;
    AnimItem1_2.drawY = 540;
    AnimItem2_2.drawX = 960;
    AnimItem2_2.drawY = 540;
    AnimItem3_2.drawX = 960;
    AnimItem3_2.drawY = 540;
    AnimItem4_2.drawX = 960;
    AnimItem4_2.drawY = 540;
    AnimItem5_2.drawX = 960;
    AnimItem5_2.drawY = 540;
    AnimItem6_2.drawX = 960;
    AnimItem6_2.drawY = 540;

    ExplaincatAnim.drawX = 960;
    ExplaincatAnim.drawY = 540;
    CheercatAnim.drawX = 960;
    CheercatAnim.drawY = 540;
    SplashAnim.drawX = 950;//あとで変動
    SplashAnim.drawY = 690;//v座標は固定
    TryAnim.drawX = 960;
    TryAnim.drawY = 540;
    WatercatAnim.drawX = 960;
    WatercatAnim.drawY = 540;

    idleAnim.loop = true;   // 待機はループ
    readyAnim.loop = false;  // 
    throwAnim.loop = false; // 投球は1回だけ
    gameBGAnim0.loop = true;
    gameBGAnim1.loop = true;
    //AnimItem0.loop = true;
    AnimItem1_1.loop = false;
    AnimItem2_1.loop = false;
    AnimItem3_1.loop = false;
    AnimItem4_1.loop = false;
    AnimItem5_1.loop = false;
    AnimItem6_1.loop = false;

    AnimItem1_2.loop = true;
    AnimItem2_2.loop = true;
    AnimItem3_2.loop = true;
    AnimItem4_2.loop = true;
    AnimItem5_2.loop = true;
    AnimItem6_2.loop = true;

    ExplaincatAnim.loop = true;
    CheercatAnim.loop = true;
    SplashAnim.loop = false;
    TryAnim.loop = false;
    WatercatAnim.loop = true;

    throwAnim.speed = 4;
    //AnimItem0.speed = 5;
    AnimItem1_1.speed = 5;
    AnimItem2_1.speed = 5;
    AnimItem3_1.speed = 5;
    AnimItem4_1.speed = 5;
    AnimItem5_1.speed = 5;
    AnimItem6_1.speed = 5;

    AnimItem1_2.speed = 5;
    AnimItem2_2.speed = 5;
    AnimItem3_2.speed = 5;
    AnimItem4_2.speed = 5;
    AnimItem5_2.speed = 5;
    AnimItem6_2.speed = 5;
    ExplaincatAnim.speed = 10;
    CheercatAnim.speed = 10;
    SplashAnim.speed = 6;
    TryAnim.speed = 6;
    WatercatAnim.speed = 11;

    //printfDx("menuBG = %d\n", menuBG);
    //DrawGraph(0, 0, idleAnim.imageHandle, TRUE);

    GameState currentState = TITLE;

    int menuIndex = 0;

    const char* menuItems[] = {
        "Start Game",
        "Options",
        "Test",
        "Test2",
        "Exit"
    };
    const char* helpText = "";
    const int menuCount = 5;
    int FrameCount = 0;
    int OldTime = GetNowCount();
    float FPS = 0;
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    while (ProcessMessage() == 0) {
        // -----------------
        // �X�V����
        // -----------------
        input.Update();
        //GetHitKeyStateAll(Key);
        FrameCount++;

        if (GetNowCount() - OldTime >= 1000) {
            FPS = (float)FrameCount;

            FrameCount = 0;
            OldTime = GetNowCount();
        }

        frameTimer++;//ゲームのループ度に+1
        if (frameTimer >= currentAnim->speed) {//時間ではなくゲームループの回数(フレーム数)10回ゲームの表示を更新したらアニメを1フレーム進める
            frameTimer = 0;
            currentFrame++;

            if (currentFrame >= currentAnim->frames.size()) {
                if (currentAnim->loop) {
                    currentFrame = 0;
                }
                else {
                    currentFrame = (int)currentAnim->frames.size() - 1;
                }
            }
        }
        frameTimer2++;

        if (frameTimer2 >= 10)
        {
            frameTimer2 = 0;
            currentFrame2++;

            if (currentFrame2 >= BGAnim->frames.size())
            {
                currentFrame2 = 0;
            }
        }

        frameTimer3++;
        if (frameTimer3 >= currentAnim3->speed) {
            frameTimer3 = 0;
            currentFrame3++;

            if (currentFrame3 >= currentAnim3->frames.size()) {
                if (currentAnim3->loop) {//ループなら
                    currentFrame3 = 0;
                }
                else {
                    currentFrame3 = (int)currentAnim3->frames.size() - 1;//6枚なら5枚目(最後のフレーム)で止まる
                }
            }
        }
        frameTimer4++;
        if (frameTimer4 >= currentAnim4->speed) {
            frameTimer4 = 0;
            currentFrame4++;

            if (currentFrame4 >= currentAnim4->frames.size()) {
                if (currentAnim4->loop) {
                    currentFrame4 = 0;
                }
                else {
                    currentFrame4 = (int)currentAnim4->frames.size() - 1;
                }
            }
        }
        switch (currentState) {
        case TITLE:

            if (input.Press(KEY_INPUT_RETURN)) {
                currentState = MENU;
            }

            break;

        case MENU:

            if (input.Press(KEY_INPUT_DOWN)) {
                menuIndex = (menuIndex + 1) % menuCount;
            }
            if (input.Press(KEY_INPUT_UP)) {
                menuIndex = (menuIndex - 1 + menuCount) % menuCount;
            }
            if (input.Press(KEY_INPUT_RETURN)) {
                if (menuIndex == 0) {
                    currentState = GAMEPLAY;
                    showInputMenu = true;
                }

                else if (menuIndex == 1)
                    currentState = OPTIONS;

                else if (menuIndex == 2)
                    currentState = TEST;

                else if (menuIndex == 3)
                    currentState = TEST2;

                else if (menuIndex == 4) {
                    DxLib_End();
                    return 0;
                }
            }
            break;

        case OPTIONS:
            break;

        case TEST:
            break;

        case TEST2:
            break;

        case GAMEPLAY:
            if (CheckSoundMem(bgmHandle) == 0) {
                PlaySoundMem(
                    bgmHandle,
                    DX_PLAYTYPE_LOOP
                );
            }
            switch (playState) {
            case COURSE_SELECT:
                //描画処理と一緒にしたからない
                break;

            case EXPLAIN:
                SetAnimationBG(BGAnim);
                SetAnimation4(&ExplaincatAnim);
                Updateexplain();

                break;

            case COURSE_PREVIEW:
                UpdateCoursePreview();
                SetAnimationBG(BGAnim);
                SetAnimation(&idleAnim);
                break;

            case COURSE_PREVIEW_ZOOMOUT:
                UpdateCoursePreviewOverview();
                SetAnimationBG(BGAnim);
                break;

            case COURSE_PREVIEW_ZOOMIN:
                UpdateCoursePreviewZoomIn();
                SetAnimationBG(BGAnim);
                break;

            case AIMING:
                UpdateAiming();
                SetAnimationBG(BGAnim);
                SetAnimation(&idleAnim);
                break;

            case WAIT_THROW:
                // UpdateWaitThrow();
                SetAnimationBG(BGAnim);
                SetAnimation(&readyAnim);
                UpdateWait();
                break;

            case THROW:
                // UpdateWaitThrow();
                SetAnimationBG(BGAnim);
                //SetAnimation(&throwAnim);
                UpdateTHrowimg();
                break;

            case THROWEND:
                SetAnimationBG(BGAnim);
                UpdateThrowend();
                //SplashAnim.drawX = ball.x;
                SetAnimation4(&CheercatAnim);//普通ならこれ
                if (lakecount) {
                    SetAnimation4(&WatercatAnim);//池に着いたなら別のねこ表示
                }
                SetAnimationITEM(&SplashAnim);//普通ならこれ
                if (lakecount) {
                    SetAnimationITEM(&SplashAnim);//池に着いたなら水しぶき表示
                }
                break;

            case ANIME0:
                SetAnimationBG(BGAnim);
                //SetAnimationITEM(&AnimItem0);
                UpAnime0();

                break;

            case RESULT:
                UpdateResult();
                SetAnimationBG(BGAnim);
                SetAnimation4(&TryAnim);
                break;
                //case ANIME1:
                //    SetAnimationBG(BGAnim);
                //    SetAnimationITEM(&AnimItem1);
                //    UpAnime1();

                //    break;

                //case ANIME2:
                //    SetAnimationBG(BGAnim);
                //    SetAnimationITEM(&AnimItem2);
                //    UpAnime2();

                //    break;

                //case ANIME3:
                //    SetAnimationBG(BGAnim);
                //    SetAnimationITEM(&AnimItem3);
                //    UpAnime3();

                //    break;

                //case ANIME4:
                //    SetAnimationBG(BGAnim);
                //    SetAnimationITEM(&AnimItem4);
                //    UpAnime4();

                //    break;

                //case ANIME5:
                //    SetAnimationBG(BGAnim);
                //    SetAnimationITEM(&AnimItem5);
                //    UpAnime5();

                //    break;

                //case ANIME6:
                //    SetAnimationBG(BGAnim);
                //    SetAnimationITEM(&AnimItem6);
                //    UpAnime6();

                //    break;
            }
            break;
        }

        // -----------------
        // 戻る系の処理
        // -----------------

        if (input.Press(KEY_INPUT_BACK)) {
            switch (currentState)
            {
            case MENU:
                currentState = TITLE;
                break;

            case OPTIONS:
                currentState = MENU;
                break;

            case TEST:
                currentState = MENU;
                break;

            case TEST2:
                currentState = MENU;
                break;

            case GAMEPLAY:
                
                //playState = COURSE_SELECT;
                course = 0;
                angleIndex = 1;
                switch (playState) {
                case COURSE_SELECT:
                    //描画処理と一緒にしたからない
                    currentState = MENU;
                    break;

                case EXPLAIN:
                    playState = COURSE_SELECT;
                    break;

                case COURSE_PREVIEW:
                    playState = COURSE_SELECT;
                    break;

                case COURSE_PREVIEW_ZOOMOUT:
                    playState = COURSE_SELECT;
                    break;

                case COURSE_PREVIEW_ZOOMIN:
                    playState = COURSE_SELECT;

                    break;

                case AIMING:
                    //playState = COURSE_SELECT;なし
                    break;

                case WAIT_THROW:
                    playState = AIMING;
                    break;

                case THROW:
                    //playState = COURSE_SELECT;なし
                    break;

                case THROWEND:
                    playState = COURSE_SELECT;
                    break;

                case ANIME0:

                    break;

                case RESULT:

                    break;
                }

            default:
                break;
            }
        }
        if (input.Press(KEY_INPUT_SPACE)) {//操作しないstateでspace押すとスキップ
            switch (playState){
            case COURSE_PREVIEW:
                playState = AIMING;
                previewCameraX = 0;
                previewScale = 1.0f;
                break;

            case COURSE_PREVIEW_ZOOMOUT:
                playState = AIMING;
                previewCameraX = 0;
                previewScale = 1.0f;
                break;

            case COURSE_PREVIEW_ZOOMIN:
                playState = AIMING;
                previewCameraX = 0;
                previewScale = 1.0f;
                break;
            }
        }
        // -----------------
        // �`�揈��
        // -----------------
        // 説明文/////////////////////////////////////////////////////////////////////////
        switch (currentState)
        {
        case TITLE:
            helpText = "--";
            break;

        case MENU:
            switch (menuIndex)
            {
            case 0:
                helpText = "ENTER : GAME";//ENTER でゲーム画面へ
                break;
            case 1:
                helpText = "ENTER : OPTION";//ENTER で設定画面へ
                break;
            case 2:
                helpText = "ENTER : TEST";//ENTER でテスト画面へ
                break;
            case 3:
                helpText = "ENTER : TEST2";//ENTER でテスト2画面へ
                break;
            case 4:
                helpText = "ENTER : EXIT";//ENTER でゲームを終了する
                break;
            }
            break;

        case OPTIONS:
            helpText = "BACKSPACE : MENU";
            break;

        case TEST:
            helpText = "BACKSPACE : MENU";
            break;

        case TEST2:
            helpText = "BACKSPACE : MENU";//BACKSPACE でメニューに戻る
            break;

        case GAMEPLAY:
            //helpText = "BACKSPACE �Ń��j���[�ɖ߂�܂�";
            break;
        }
        /////////////////////////////////////////////////////////////////////////////////////////
        char fpsText[32];


        ClearDrawScreen();

        switch (currentState) {
        case TITLE:

            DrawString(
                (1920 - GetDrawStringWidth("THROWING GOLF", strlen("THROWING GOLF"))) / 2,
                500,
                "THROWING GOLF",
                0xffffff);//GetColor(255, 255, 255));

            DrawString(
                (1920 - GetDrawStringWidth("Press ENTER", strlen("Press ENTER"))) / 2,
                550,
                "Press ENTER",
                GetColor(255, 255, 0));
            break;

        case MENU:
            DrawGraph(0, 0, menuBG, TRUE);
            for (int i = 0; i < menuCount; i++)
            {
                int color =
                    (i == menuIndex)
                    ? GetColor(255, 255, 0)
                    : GetColor(255, 255, 255);

                DrawString(
                    (1920 - GetDrawStringWidth(menuItems[i], strlen(menuItems[i]))) / 2,
                    390 + i * 50,
                    menuItems[i],
                    color);
            }
            break;

        case OPTIONS:

            DrawString(
                (1920 - GetDrawStringWidth("OPTIONS", strlen("OPTIONS"))) / 2,
                500,
                "OPTIONS",
                GetColor(255, 255, 255));
            break;

        case TEST:

            DrawString(
                (1920 - GetDrawStringWidth("TEST", strlen("TEST"))) / 2,
                500,
                "TEST",
                GetColor(255, 255, 255));
            break;

        case TEST2:
            DrawBox(0, 0, 1920, 1080, GetColor(255, 255, 255), TRUE);

            DrawString(
                (1920 - GetDrawStringWidth("TEST2", strlen("TEST2"))) / 2,
                500,
                "TEST2",
                GetColor(255, 255, 255));

            DrawCharacter();

            printfDx("OK?\n");
            DrawBitmapString(
                100,
                100,
                "ABC"
            );

            break;

        case GAMEPLAY:
            ///////共通部分
           // helpText = "Back space でMenuに戻ります";
            //DrawGraph(0, 0, gameBG, TRUE);
            DrawAnimetion2();
            DrawGraph(0, 0, hederImg, TRUE);


            switch (playState) {
            case COURSE_SELECT:
                helpText = "up/down: Select   Space : Confirm";//矢印キーで選択、Space で決定
                DrawCourseSelect();
                break;

            case EXPLAIN:
                helpText = "SPACE : NEXT";
                DrawProgressMap();
                DrawCourse();
                SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
                DrawBox(0, 0, 1920, 1080, GetColor(0, 0, 0), TRUE);
                SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
                DrawAnimetion4();//ねこあにめ
                DrawExplain();//ふきだし
                break;

            case COURSE_PREVIEW:
                helpText = "SPACE : NEXT";
                DrawProgressMap();
                DrawCoursePreview();
                break;

            case COURSE_PREVIEW_ZOOMOUT:
                helpText = "SPACE : NEXT";
                DrawProgressMap();
                DrawCoursePreviewOverview();
                break;

            case COURSE_PREVIEW_ZOOMIN:
                helpText = "SPACE : NEXT";
                DrawProgressMap();
                DrawCoursePreviewZoomIn();
                break;

            case AIMING://角度選択
                helpText = "up/down : Select  right/left : View Course   Space : Confirm   ";
                DrawProgressMap();
                DrawCourse();
                DrawCharacter();
                DrawAiming();
                DrawData();
                break;

            case WAIT_THROW:
                helpText = "THROW";
                DrawProgressMap();
                DrawCourse();
                DrawCharacter();
                DrawData();
                break;

            case THROW:
                helpText = "";
                DrawProgressMap();
                DrawCourse();
                DrawCharacter();
                DrawBall();
                DrawData();
                break;

            case THROWEND:
                helpText = "SPACE : NEXT";
                DrawProgressMap();
                DrawAnimetion3();
                DrawCourse();
                if (!lakecount)Throwend();//ただの旗の描画  池に入ったらいらない

                DrawCharacter();
                DrawResult();//DrawAnimetion4();が含まれる

                //DrawAnimetion3();
                DrawData();

                break;

            case RESULT:
                helpText = "SPACE : NEXT";
                DrawProgressMap();
                DrawCourse();
                DrawResulting();
                DrawData();
                break;

            case ANIME0:
                DrawProgressMap();
                DrawCourse();
                DrawCharacter();
                //DrawBall();
                DrawRect();
                //DrawAnimetion3();
                //DrawAnimetion4();
                break;

                //case ANIME1:
                //    DrawProgressMap();
                //    DrawCourse();
                //    DrawCharacter();
                //    DrawBall();
                //    DrawAnimetion3();
                //    break;

                //case ANIME2:
                //    DrawProgressMap();
                //    DrawCourse();
                //    DrawCharacter();
                //    DrawBall();
                //   DrawAnimetion3();
                //    break;

                //case ANIME3:
                //    DrawProgressMap();
                //    DrawCourse();
                //    DrawCharacter();
                //    DrawBall();
                //    DrawAnimetion3();
                //    break;

                //case ANIME4:
                //    DrawProgressMap();
                //    DrawCourse();
                //    DrawCharacter();
                //    DrawBall();
                //    DrawAnimetion3();
                //    break;

                //case ANIME5:
                //    DrawProgressMap();
                //    DrawCourse();
                //    DrawCharacter();
                //    DrawBall();
                //    DrawAnimetion3();
                //    break;

                //case ANIME6:
                //    DrawProgressMap();
                //    DrawCourse();
                //    DrawCharacter();
                //    DrawBall();
                //    DrawAnimetion3();
                //    break;
            }
            break;
        }
        //SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
        //DrawBox(0, 0, 1920, 1080, GetColor(0, 0, 0), TRUE);
        //SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        DrawBox(
            0,
            1020,
            1920,
            1080,
            GetColor(30, 30, 30),
            TRUE
        );
        DrawString(
            20,
            1040,
            helpText,
            GetColor(255, 255, 255)
        );
        sprintf_s(
            fpsText,
            "FPS : %.0f",
            FPS
        );

        DrawString(
            10,
            10,
            fpsText,
            GetColor(255, 255, 255)
        );
        ScreenFlip();
        //memcpy(OldKey, Key, 256);

    }
    DeleteGraph(menuBG);
    DxLib_End();
    return 0;
}
