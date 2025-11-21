import socket from '../socket';

const CarControls = () => {
  const sendCar = (cmd) => {
    socket.emit('car_move', cmd);
  };

  const stopCar = () => {
    socket.emit('car_move', '0');
  };

  // Styling for the D-Pad layout
  const btnStyle = { padding: '15px 25px', margin: '5px', fontSize: '1.2rem', cursor: 'pointer' };

  return (
    <div className="control-panel">
      <h3>Car Controls</h3>
      <div>
        <button style={btnStyle} onMouseDown={() => sendCar('1')} onMouseUp={stopCar}>▲</button>
      </div>
      <div>
        <button style={btnStyle} onMouseDown={() => sendCar('3')} onMouseUp={stopCar}>◄</button>
        <button style={{...btnStyle, backgroundColor: '#ff4444'}} onMouseDown={stopCar}>STOP</button>
        <button style={btnStyle} onMouseDown={() => sendCar('4')} onMouseUp={stopCar}>►</button>
      </div>
      <div>
        <button style={btnStyle} onMouseDown={() => sendCar('2')} onMouseUp={stopCar}>▼</button>
      </div>
    </div>
  );
};

export default CarControls;