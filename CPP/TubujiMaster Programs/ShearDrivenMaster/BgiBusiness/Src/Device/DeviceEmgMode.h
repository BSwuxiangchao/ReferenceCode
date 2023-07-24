/*
* @简    述：设备急停控制类；
* @详    细：设备急停控制类；
* @继    承：无
* @头 文 件：DeviceEmgMode.h
* @实现文件：DeviceEmgMode.cpp
* @上一版本：1.0
* @作    者：xiechengwang
* @创建时间：2022/7/26 11:12
* @当前版本：1.0
* @修改时间：无
*
*/

// 使用宏定义保护头文件不被重复包含
#pragma once
// 包含系统文件

// 包含项目文件

// 包含局部头文件

// 放置前置引用

class DeviceEmgMode
{
public:
	// 这里放置类的生命期控制函数，例如构造，析构函数，拷贝构造函数
	DeviceEmgMode();

	// 这里放置类的运算操作符函数，=，==等

	// 这里放置类的操作

	// 设置急停
	int SetEmg();
	// 获取设置急停状态
	bool GetEmg(unsigned short& enable, unsigned short& logic);
	// 清除急停设置；
	int ClearEmg();

	// 这里放置属性的存取
	// 设置参数
	void SetEmgParam(unsigned short iCarNo, unsigned short iMapIoIndex, unsigned short iAxis, unsigned short logic);

	//运动卡的编号
	void SetCardNo(const unsigned short& cardNo);
	unsigned short GetCardNo() const;

	// 轴号
	void SetAxis(const unsigned short& axis);
	unsigned short GetAxis() const;

	// 轴IO映射索引号
	void SetMapIoIndex(const unsigned short& io);
	unsigned short GetMapIoIndex() const;


	// 急停电平
	void SetEmgLogic(const unsigned short& logic);
	unsigned short GetEmgLogic() const;

	// 这里放置类的状态查询

protected:
	//运动卡的编号
	unsigned short m_iCardNo;
	// 指定轴的 IO 信号类型为：急停信号
	unsigned short m_iIoType;
	// 轴 IO 映射类型：通用输入端
	unsigned short m_iMapIoType;
	// 轴 IO 映射索引号：通用输入
	unsigned short m_iMapIoIndex;
	// 滤波时间间隔
	double m_fFilter;

	// 轴号
	unsigned short m_iAxis;
	// 急停电平
	unsigned short m_iEmgLogic;


private:

};

