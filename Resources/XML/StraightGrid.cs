using System.Linq;

namespace OverCraft.Data {
	public class StraightGrid : Grid {

		public StraightGrid(int Color = 0) {
			_color = Color;
		}

		public override Grid NextGrid(Plane plane = null) {
			return plane == null ? _nextGrid : (plane.PrevGrid != _nextGrid ? _nextGrid : _prevGrid);
		}

		public override void Check(Plane plane, bool Jumped = false) {
			checkEat(plane);
			if (Color == plane.Color)
				_planeList.Add(plane);
			plane.removePrev();
		}
	}
}
