
const mongoose = require('mongoose');
const mqtt = require('mqtt');
const express = require('express');
const cors = require('cors');


const MONGO_URI = "mongodb+srv://xiarenee17_db_user:Sc0ZXq96gz3RwPxx@cluster0.4i36bjl.mongodb.net/?appName=Cluster0mongodb+srv://admin:1234@cluster0.xxxxx.mongodb.net/iot_project?retryWrites=true&w=majority";

// MQTT 配置
const MQTT_HOST = 'mqtt://broker.emqx.io';
const MQTT_TOPIC = 'building/toilet/smoke_detector';


const app = express();
app.use(cors()); 
const HTTP_PORT = 3000;


const RecordSchema = new mongoose.Schema({
    timestamp: { type: Date, default: Date.now },
    temp: Number,
    hum: Number,
    smoke: Number,
    status: String,
    type: String,
    alert: Boolean
});
const Record = mongoose.model('SensorData', RecordSchema);

mongoose.connect(MONGO_URI)
    .then(() => console.log('✅ MongoDB 已连接'))
    .catch(err => console.error('❌ DB 连接失败:', err));

const mqttClient = mqtt.connect(MQTT_HOST);

mqttClient.on('connect', () => {
    console.log('✅ MQTT 已连接 (监听存储中...)');
    mqttClient.subscribe(MQTT_TOPIC);
});

mqttClient.on('message', async (topic, message) => {
    try {
        const data = JSON.parse(message.toString());
        const newRecord = new Record(data); // 存入
        await newRecord.save();
        console.log(`💾 [存储] 烟雾值:${data.smoke} | 类型:${data.type}`);
    } catch (e) {
        console.error('存储错误:', e);
    }
});




app.get('/api/history', async (req, res) => {
    try {
        const range = req.query.range || '24h'; 
        let startTime = new Date();

   
        if (range === '7d') {
            startTime.setDate(startTime.getDate() - 7); 
            console.log(`🔍 [API] 查询最近 7 天的数据`);
        } else {
            startTime.setHours(startTime.getHours() - 24); 
            console.log(`🔍 [API] 查询最近 24 小时的数据`);
        }

        
        const history = await Record.find({ 
            timestamp: { $gte: startTime } 
        })
        .sort({ timestamp: 1 }) /

        res.json(history);
        
    } catch (e) {
        console.error(e);
        res.status(500).json({ error: e.message });
    }
});


app.listen(HTTP_PORT, () => {
    console.log(`🚀 API 服务器运行在: http://localhost:${HTTP_PORT}`);
});