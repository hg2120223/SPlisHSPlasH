import pysplishsplash as sph
from pysplishsplash.Extras import Scenes
import os

def main():
    base = sph.Exec.SimulatorBase()
    output_dir = os.path.abspath("results/ferro-50k")
    base.init(useGui=False, outputDir=output_dir, sceneFile="/home/jake/Documents/fluids/SPlisHSPlasH/data/Scenes/HydrostaticTank.json")
    base.setValueFloat(base.STOP_AT, 20.0) # Important to have the dot to denote a float
    base.setValueBool(base.PARTIO_EXPORT, True)
    # Uncomment the next line to set the output FPS value (must be float)
    # base.setValueFloat(base.DATA_EXPORT_FPS, 10000.) 
    base.run()

if __name__ == "__main__":
    main()
