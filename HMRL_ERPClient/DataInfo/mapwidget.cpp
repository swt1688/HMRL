#include "mapwidget.h"
#include "ui_mapwidget.h"

#include <QSqlQuery>
#include <QDir>
#include <QDebug>
#include <QTextCodec>
#include "logoin.h"

MapWidget::MapWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MapWidget)
{
    ui->setupUi(this);

//       manager=new QNetworkAccessManager(this);
//       QObject::connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(replyFinished(QNetworkReply*)));
//       manager->get(QNetworkRequest(QUrl("http://www.baidu.com")));

       ui->addrLineEdit->setText("http://www.baidu.com");
       ui->webBrowser->setProperty("DisplayAlerts",false); //不显示任何警告信息。
       this->showAddrWeb();

       connect(ui->goPushButton,SIGNAL(clicked(bool)),this,SLOT(showAddrWeb()));
       connect(ui->mapBtn,SIGNAL(clicked(bool)),this,SLOT(showBaiduMap()));
}

MapWidget::~MapWidget()
{
    delete ui;
}

//void MapWidget::replyFinished(QNetworkReply *reply)
//{
//    QString all = reply->readAll();
//    ui->addrLineEdit->setText(all);
//    reply->deleteLater();
//}

//QUrl  MapWidget::GPSMark(const QString &GPSCoordinate)
//{


//QUrl temp;
//QString ret;
//HttpClient * http = new HttpClient();
//QUrl url;
//url.setUrl("http://cache.amap.com/lbs/static/main1119.css");
//QUrlQuery urlQuery;
////urlQuery.addQueryItem("location", GPSCoordinate);
////urlQuery.addQueryItem("title", tr("my GPS location"));  //标点的标题
////urlQuery.addQueryItem("content", tr("current location"));       //标点的内容
////urlQuery.addQueryItem("output", "html");
////urlQuery.addQueryItem("coord_type", "bd09");
////urlQuery.addQueryItem("src", "map");
//    url.setQuery(urlQuery);
////http://map.baidu.com/?latlng=40.047669,116.313082&title=my gps loclation
////url = QUrl("http://map.baidu.com/?latlng=40.047669,116.313082&title=我的位置&content=百度奎科大厦&autoOpen=true&l");
////webview->load(url);
////webview->show();


//setWindowTitle(url.toString());
//return temp;
//}

void MapWidget::webShow(const QString &url)
{
    ui->webBrowser->dynamicCall("Navigate(const QString&)", url);
}

void MapWidget::showAddrWeb()
{
    QString addr=ui->addrLineEdit->text().trimmed();
    //if((addr.indexOf("www")!=0)&&(addr.indexOf("http")!=0))  return;
    webShow(addr);
}

void MapWidget::showBaiduMap()
{
    QString filePath=Logoin::AppPath+"/MapFile/BaiduMap.html";

    QString strs;
    QStringList LocationInfo,LocationData;
    strs="SELECT CompanyName,CompanyAddress,RegulatorLevel,City,Town,Longitudo,Latitude FROM `onlinedatabase`.enterpriseinfo where CompanyType='排污单位';";
    QSqlQuery query;
    query.exec(strs);
    while(query.next())
    {
        if((!query.value(5).toString().isEmpty())&&(!query.value(6).toString().isEmpty()))
        {
            LocationInfo<<tr("[\"%1\",\"%2\"]").arg(query.value(5).toString()).arg(query.value(6).toString());
            LocationData<<tr("{\"CompanyName\":\"%1\",\"ComapanyAddress\":\"%2\",\"RegulatorLevel\":\"%3\",\"Location\":\"%4 %5\"}").arg(query.value(0).toString()).arg(query.value(1).toString()).arg(query.value(2).toString()).arg(query.value(3).toString()).arg(query.value(4).toString());
        }
    }
    QString mapHtml;
    mapHtml.append("<!DOCTYPE html>\r\n <html>\r\n\t <head> <meta charset=\"utf-8\">\r\n");
    mapHtml.append("<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\r\n");
    mapHtml.append("<meta name=\"viewport\" content=\"initial-scale=1.0, user-scalable=no, width=device-width\">\r\n");
    mapHtml.append("<title>地理信息</title>\r\n");
    mapHtml.append("<script src=\"/static/jquery/jquery-1.11.1.min.js\"></script>\r\n");
    mapHtml.append("<link href=\"/static/materialize/css/materialize.min.css\" rel=\"stylesheet\">\r\n");
    mapHtml.append("<script src=\"/static/materialize/js/materialize.min.js\"></script>");
    mapHtml.append("<link rel=\"stylesheet\" href=\"http://cache.amap.com/lbs/static/main1119.css\"/>\r\n");
    mapHtml.append("<script src=\"http://webapi.amap.com/maps?v=1.4.4&key=7c00596588f1e696b7d2b1f9d503815f&plugin=AMap.Autocomplete,AMap.PlaceSearch\"></script>\r");//<!--引入高德地图api，key为你自己获取的key值-->
    mapHtml.append("<script type=\"text/javascript\" src=\"http://cache.amap.com/lbs/static/addToolbar.js\"></script>\r\n");
    mapHtml.append("<style> .amap-logo{ display: none;    }.amap-copyright{ display: none!important;}    </style> \r\n");/*隐藏左下角地图logo*/
    mapHtml.append("<style type=\"text/css\">.info {border: solid 1px silver;}div.info-top {position: relative;background: none repeat scroll 0 0 #F9F9F9;border-bottom: 1px solid #CCC;border-radius: 5px 5px 0 0;}"
                   "div.info-top div {display: inline-block;color: #333333;font-size: 14px;font-weight: bold;line-height: 31px;padding: 0 10px;}"
                   "div.info-top img {position: absolute;top: 10px;right: 10px;transition-duration: 0.25s;}"
                   "div.info-top img:hover {box-shadow: 0px 0px 5px #000;}"
                   "div.info-middle {font-size: 12px;padding: 6px;line-height: 20px;}"
                   "div.info-bottom {height: 0px;width: 100%;clear: both;text-align: center;}"
                   "div.info-bottom img {position: relative;z-index: 104;}"
                   "span {margin-left: 5px;font-size: 11px;}.info-middle img {float: left;margin-right: 6px;}</style>\r\n");
    mapHtml.append("</head><body> <div id=\"container\" style=\"width:100%;height:100%\"></div>\r\n");
    mapHtml.append("<div id=\"myPageTop\"><table><tr><td><label>按关键字搜索：</label></td><td class=\"column2\"><label>左击获取经纬度：</label></td></tr>"
                   "<tr><td><input type=\"text\" placeholder=\"请输入关键字进行搜索\" id=\"tipinput\"></td><td class=\"column2\"><input type=\"text\" readonly=\"true\" id=\"lnglat\"></td></tr></table></div>");
    mapHtml.append("<div id=\"tip\" style=\"line-height: 24px\"></div><div class=\'button-group\' style=\"background-color: #0d9bf2\">"
                   "<input type=\'radio\' onclick=\'refresh(this.value)\' checked name=\'mapStyle\' value=\'normal\'>normal"
                   "<input type=\'radio\' onclick=\'refresh(this.value)\' name=\'mapStyle\' value=\'blue_night\'>blue_night"
                   "<input type=\'radio\' onclick=\'refresh(this.value)\' name=\'mapStyle\' value=\'light\'>light"
                   "<input type=\'radio\' onclick=\'refresh(this.value)\' name=\'mapStyle\' value=\'fresh\'>fresh"
                   "<input type=\'radio\' onclick=\'refresh(this.value)\' name=\'mapStyle\' value=\'dark\'>dark</div>");
    mapHtml.append("<script>\r\n\t");
    mapHtml.append("var map = new AMap.Map('container', {pitch:45,viewMode:'3D',zoom: 12,expandZoomRange:true,zooms:[3,20],resizeEnable: true});\r\t ");
    mapHtml.append("map.plugin([\"AMap.ToolBar\"], function() {map.addControl(new AMap.ToolBar());});\r\n");
    mapHtml.append("var infoWindow = new AMap.InfoWindow({});\r\n");//创建信息窗口对象  ps.高德目前不支持多信息窗口，即使创建多个窗口对象，也只会显示一个
    mapHtml.append("function refresh(e) {map.setMapStyle(e); }");

    mapHtml.append(tr("var lnglats= [%1];\r\n").arg(LocationInfo.join(",")));
    mapHtml.append(tr("var data = [%1];\r\n").arg(LocationData.join(",")));

    mapHtml.append("for(var i= 0;i<lnglats.length;i++) \r\n {var marker=new AMap.Marker({position:lnglats[i],map:map});\r\n");
    mapHtml.append("content = [];\r\n content.push(\"公司名称：\"+data[i].CompanyName);\r\n content.push(\"公司地址：\"+data[i].ComapanyAddress);\r\n content.push(\"监管级别：\"+data[i].RegulatorLevel);\r\n content.push(\"区域：\"+data[i].Location); \r\n marker.content = content;\r\n marker.title = data[i].CompanyName+\".\"+data[i].RegulatorLevel;\r\n");//标记点的title
    mapHtml.append("infoWindow.setContent(content.join(\"<br/>\"));\r\ninfoWindow.open(map,lnglats[i]);\r\n");
    mapHtml.append(" marker.on(\'dblclick\',openAmap);     \r\n");//这里采用调到新页面方式导航，也可直接定义带导航功能的信息窗体，
    mapHtml.append("marker.on(\'click\', markerClick);} \r\n");//给Marker绑定单击事件
    mapHtml.append("var clickEventListener = map.on(\'click\', function(e) {document.getElementById(\"lnglat\").value = e.lnglat.getLng() + \',\' + e.lnglat.getLat()});\r");/*为地图注册click事件获取鼠标点击出的经纬度坐标*/
    mapHtml.append("var autoOptions = { input: \"tipinput\" };\r var auto = new AMap.Autocomplete(autoOptions);var placeSearch = new AMap.PlaceSearch({map: map});");  //构造地点查询类
    mapHtml.append("AMap.event.addListener(auto, \"select\", select);function select(e) {placeSearch.setCity(e.poi.adcode);placeSearch.search(e.poi.name);}");//注册监听，当选中某条记录时会触发,关键字查询查询

    mapHtml.append("map.setFitView(); \r");
    mapHtml.append(" function openAmap(e) \r\n {e.target.markOnAMAP({name:e.target.title,position:e.target.getPosition()})} \r\n");//跳至地图当中导航
    mapHtml.append("function markerClick(e) \r\n {infoWindow.setContent(e.target.content.join(\"<br/>\"));infoWindow.open(map, e.target.getPosition());} \r\n");//信息窗口
    //mapHtml.append("map.on(\'complete\', function() {Materialize.toast(\'地图图块加载完毕！\', 4000)}); \r\n"); //        document.getElementById('tip').innerHTML = "地图图块加载完毕！当前地图中心点为：" + data[0].name;
    mapHtml.append("</script> \r\n </body> \r\n </html> \r\n");


    QFile ExpandData(filePath);
    QTextStream Out(&ExpandData);
       if(ExpandData.open(QIODevice::WriteOnly | QIODevice::Text ))
       {
           /*文本输出流，用于保存数据*/
           /*加入空格的好处是以流的形式读取恰好是相反的操作，这样一来方便快速对参数的读取*/
           Out.setCodec("utf-8");
           Out <<mapHtml;
           ExpandData.close();
       }



    qDebug()<<Out.readAll();

    ui->addrLineEdit->setText("地图");
    webShow(filePath);
}


