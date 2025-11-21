import { useEffect, useRef, useState } from 'react';
import { Hands, HAND_CONNECTIONS } from '@mediapipe/hands';
import { Camera } from '@mediapipe/camera_utils';
import { drawConnectors, drawLandmarks } from '@mediapipe/drawing_utils';
import socket from '../socket';

const VisionControl = () => {
  const videoRef = useRef(null);
  const canvasRef = useRef(null);
  const [isTracking, setIsTracking] = useState(false);
  const [debugData, setDebugData] = useState({ b: 90, s: 90, e: 90, g: 0 });
  
  // Refs to store non-react state variables for loop performance
  const lastSentRef = useRef(0);
  const isTrackingRef = useRef(false); 

  // Sync ref with state
  useEffect(() => {
    isTrackingRef.current = isTracking;
  }, [isTracking]);

  useEffect(() => {
    const videoElement = videoRef.current;
    const canvasElement = canvasRef.current;
    const canvasCtx = canvasElement.getContext('2d');

    const onResults = (results) => {
      // 1. Draw Video & Skeleton
      canvasCtx.save();
      canvasCtx.clearRect(0, 0, canvasElement.width, canvasElement.height);
      canvasCtx.drawImage(results.image, 0, 0, canvasElement.width, canvasElement.height);

      if (results.multiHandLandmarks && results.multiHandLandmarks.length > 0 && isTrackingRef.current) {
        const landmarks = results.multiHandLandmarks[0];
        
        drawConnectors(canvasCtx, landmarks, HAND_CONNECTIONS, { color: '#00FF00', lineWidth: 5 });
        drawLandmarks(canvasCtx, landmarks, { color: '#FF0000', lineWidth: 2 });

        // 2. Calculate Angles
        // Base: Wrist X (inverted)
        let b = Math.floor((1 - landmarks[0].x) * 180);
        
        // Shoulder: Wrist Y
        let s = Math.floor((1 - landmarks[0].y) * 160);

        // Elbow: Height Diff between Wrist(0) and Index Tip(8)
        let diff = landmarks[0].y - landmarks[8].y;
        let e = Math.floor(90 + (diff * 200));

        // Gripper: Thumb(4) to Index(8) dist
        let dX = landmarks[4].x - landmarks[8].x;
        let dY = landmarks[4].y - landmarks[8].y;
        let dist = Math.sqrt(dX*dX + dY*dY);
        let g = dist < 0.05 ? 0 : 90;

        // Constrain
        b = Math.max(0, Math.min(180, b));
        s = Math.max(15, Math.min(160, s));
        e = Math.max(0, Math.min(180, e));

        // Update UI Debug
        setDebugData({ b, s, e, g });

        // 3. Send to Server (Throttled 500ms)
        const now = Date.now();
        if (now - lastSentRef.current > 500) {
          socket.emit('arm_data', { b, s, e, g });
          lastSentRef.current = now;
        }
      }
      canvasCtx.restore();
    };

    // Initialize MediaPipe Hands
    const hands = new Hands({
      locateFile: (file) => `https://cdn.jsdelivr.net/npm/@mediapipe/hands/${file}`,
    });

    hands.setOptions({
      maxNumHands: 1,
      modelComplexity: 1,
      minDetectionConfidence: 0.5,
      minTrackingConfidence: 0.5,
    });

    hands.onResults(onResults);

    // Initialize Camera
    const camera = new Camera(videoElement, {
      onFrame: async () => {
        await hands.send({ image: videoElement });
      },
      width: 640,
      height: 480,
    });

    camera.start();

    return () => {
        // Cleanup if component unmounts (optional)
        // camera.stop(); 
    };
  }, []);

  return (
    <div className="vision-panel">
      <div className="video-container" style={{ position: 'relative', width: 640, height: 480, margin: '0 auto' }}>
        <video ref={videoRef} style={{ display: 'none' }}></video>
        <canvas 
          ref={canvasRef} 
          width={640} 
          height={480} 
          style={{ width: '100%', transform: 'scaleX(-1)', border: '2px solid #00ff00' }} 
        />
      </div>

      <div className="hud">
        <button onClick={() => setIsTracking(!isTracking)} style={{ marginTop: 20, padding: '10px 20px', fontSize: '1.2rem' }}>
            {isTracking ? "STOP TRACKING" : "START TRACKING (CV)"}
        </button>
        
        <div style={{ marginTop: 10, fontFamily: 'monospace', fontSize: '1.2rem' }}>
            Base: {debugData.b} | Shoulder: {debugData.s} | Elbow: {debugData.e} | Grip: {debugData.g}
        </div>
      </div>
    </div>
  );
};

export default VisionControl;