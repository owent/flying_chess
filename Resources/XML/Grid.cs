using System.Collections.Generic;
using System.Linq;

namespace OverCraft.Data {
	public class Grid {

		public Grid(int Color = 0) {
			_color = Color;
			_planeList = new List<Plane>();
		}

		protected List<Plane> _planeList;

		public int Color {
			get {
				return _color;
			}
		}
		protected int _color;

		protected Grid _prevGrid;
		protected Grid _nextGrid;
		protected Grid _jumpGrid;

		public int PositionX;
		public int PositionY;

		public Grid SetPrevGrid {
			set {
				_prevGrid = value;
			}
		}

		public Grid SetNextGrid {
			set {
				_nextGrid = value;
			}
		}

		public Grid SetJumpGrid {
			set {
				_jumpGrid = value;
			}
		}

		public virtual Grid NextGrid(Plane plane = null) {
			return _nextGrid;
		}

		public Grid JumpGrid {
			get {
				return _jumpGrid;
			}
		}

		protected virtual bool checkJump(Plane plane) {
			if (plane.Color == Color) {
				plane.Jump();
				return true;
			}
			return false;
		}

		protected virtual void checkEat(Plane plane) {
			var t = new List<Plane>();
			foreach (var tmpPlane in _planeList.Where(tmpPlane => tmpPlane.Color != plane.Color)) {
				tmpPlane.GoHome();
				t.Add(tmpPlane);
			}
			t.Count(tmpPlane => DeletePlane(tmpPlane));
		}

		public virtual void Check(Plane plane, bool jumped = false) {
			if (!jumped && checkJump(plane)) {
				return;
			}
			checkEat(plane);
			_planeList.Add(plane);
		}

		public bool DeletePlane(Plane plane) {
			while (_planeList.Remove(plane))
				;
			return true;
		}
	}
}
