
namespace OverCraft.Data {
	public class TargetGrid : StraightGrid {

		public TargetGrid(int Color = 0) {
			_color = Color;
		}

		public override Grid NextGrid(Plane plane = null) {
			return _prevGrid;
		}

		protected virtual void checkComplete(Plane plane) {
			plane.Complete();
			DeletePlane(plane);
		}

		public override void Check(Plane plane, bool Jumped = false) {
			checkComplete(plane);
		}
	}
}
