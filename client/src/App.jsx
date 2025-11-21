import './App.css';
import CarControls from './components/CarControls';
import VisionControl from './components/VisionControl';

function App() {
  return (
    <div className="App">
      <h1>🤖 Robot AI Controller</h1>
      <p>React + Node + Arduino + Computer Vision</p>
      
      <div className="main-container">
        <VisionControl />
        <CarControls />
      </div>
    </div>
  );
}

export default App;