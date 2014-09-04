
namespace OverCraft.Data {
	public class AlterGrid : Grid {

		public AlterGrid(int Color = 0) {
			_color = Color;
		}

		protected Grid _straightGrid;

		public Grid SetStraightGrid {
			set {
				_straightGrid = value;
			}
		}

		public override Grid NextGrid(Plane plane = null) {
			return plane == null ? _nextGrid : (plane.Color != Color ? _nextGrid : _straightGrid);
		}
	}
}
