import pysplishsplash as sph
from pysplishsplash.Extras import Scenes
from pathlib import Path

def main():
    base = sph.Exec.SimulatorBase()
    # base.activateExporter("Partio Exporter", True)
    output_dir = str(Path(__file__).resolve().parents[2]) + "/results/ferro-50k"
    scene_file = str(Path(__file__).resolve().parents[2]) + "/data/Scenes/HydrostaticTank.json"
    base.init(useGui=False, outputDir=output_dir, sceneFile=scene_file)
    base.setValueFloat(base.STOP_AT, 20.) # Important to have the dot to denote a float
    base.activateExporter("Partio Exporter", True)
    base.setValueFloat(base.DATA_EXPORT_FPS, 25.) # Important to have the dot to denote a float. Default: 25
    base.run()

if __name__ == "__main__":
    main()
