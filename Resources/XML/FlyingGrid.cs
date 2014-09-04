
namespace OverCraft.Data {
	public class FlyingGrid : Grid {

		public FlyingGrid(int Color = 0) {
			_color = Color;
		}

		protected Grid _crossGrid;

		public Grid SetCrossGrid {
			set {
				_crossGrid = value;
			}
		}

		public Grid CrossGrid {
			get {
				return _crossGrid;
			}
		}

		protected override bool checkJump(Plane plane) {
			if (plane.Color == Color) {
				plane.Jump(true);
				return true;
			}
			return false;
		}
	}
}
